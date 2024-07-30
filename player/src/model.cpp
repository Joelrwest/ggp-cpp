#include "../include/model.hpp"

#include <torch/script.h>

namespace player
{
inline static const torch::Device device{torch::cuda::is_available() ? torch::kCUDA : torch::kCPU};
inline static const torch::ScalarType dtype{torch::kFloat64};

ReplayBuffer::ReplayBuffer(const propnet::Propnet &propnet) : propnet{propnet}, buffer{}
{
}

ReplayBuffer::Item::Item(propnet::State state, std::vector<Policy> policies, std::vector<ExpectedValue> evs)
    : state{state.to_vec<double>()}, policies{std::move(policies)}, evs{std::move(evs)}
{
}

ReplayBuffer::Sample ReplayBuffer::sample(std::size_t sample_size) const
{
    const auto max_policy_size{propnet.get_max_policy_size()};
    const auto state_size{static_cast<std::int64_t>(propnet.size())};
    const auto num_players{static_cast<std::int64_t>(propnet.num_player_roles())};

    std::vector<ReplayBuffer::Item> sample_items{};
    std::sample(buffer.begin(), buffer.end(), std::back_inserter(sample_items), sample_size,
                std::mt19937{std::random_device{}()});

    std::vector<torch::Tensor> states{};
    std::vector<torch::Tensor> policies{};
    std::vector<torch::Tensor> evs{};
    for (auto &sample_item : sample_items)
    {
        auto state{torch::from_blob(sample_item.state.data(), {state_size}, torch::TensorOptions().dtype(dtype))};
        state = state.to(device);
        states.push_back(std::move(state));

        std::vector<double> linear_policies{};
        for (auto &policy : sample_item.policies)
        {
            std::transform(policy.begin(), policy.end(), std::back_inserter(linear_policies),
                           [](const auto &entry) { return entry.second; });
            const auto num_fill{max_policy_size - policy.size()};
            std::fill_n(std::back_inserter(linear_policies), num_fill, 0.0);
        }
        auto policy{torch::from_blob(linear_policies.data(), {num_players, static_cast<std::int64_t>(max_policy_size)},
                                     torch::TensorOptions().dtype(dtype))};
        policy = policy.to(device);
        policies.push_back(std::move(policy));

        auto ev{torch::from_blob(sample_item.evs.data(), {num_players}, torch::TensorOptions().dtype(dtype))};
        ev = ev.to(device);
        evs.push_back(std::move(ev));
    }

    return {.states = torch::stack(states), .policies = torch::stack(policies), .evs = torch::stack(evs)};
}

std::size_t ReplayBuffer::size() const
{
    return buffer.size();
}

Network::Network(const propnet::Propnet &propnet)
    : input_size{propnet.size()}, hidden_layer_size{2 * input_size},
      features_head{register_module(FEATURES_HEAD_NAME, make_features_head(input_size, hidden_layer_size))},
      evs_head{register_module(EVS_HEAD_NAME, make_evs_head(hidden_layer_size, propnet.num_player_roles()))},
      common_policy_head{register_module(COMMON_POLICY_HEAD_NAME, make_common_policy_head(hidden_layer_size))},
      policy_heads{}
{
    const auto max_policy_size{propnet.get_max_policy_size()};
    for (auto head_count{0u}; head_count < propnet.num_player_roles(); ++head_count)
    {
        std::stringstream module_name{};
        module_name << POLICY_HEAD_PREFIX << head_count;
        policy_heads.emplace_back(register_module(module_name.str(), make_policy_head(max_policy_size)));
    }
}

Network::Eval Network::forward(torch::Tensor x)
{
    /*
    Extract common features
    */
    x = features_head->forward(x);

    /*
    Get evs for each role
    */
    auto evs{evs_head->forward(x)};

    /*
    Get policies for each role
    */
    /*
    TODO: Not yet implemented...
    const auto multiple_states{x.dim() > 1};

    Common extra layer before each role gets a policy
    x = common_policy_head->forward(x);

    std::vector<torch::Tensor> policies{};
    for (auto &policy_head : policy_heads)
    {
        auto policy{policy_head->forward(x).to(dtype)};
        policies.push_back(multiple_states ? policy.unsqueeze(1) : std::move(policy));
    }

    return {.evs = std::move(evs), .policies = multiple_states ? torch::cat(policies, 1) : torch::stack(policies)};
    */

    return {.evs = std::move(evs), .policies = torch::zeros(0)};
}

torch::nn::Sequential Network::make_features_head(std::size_t input_size, std::size_t hidden_layer_size)
{
    return torch::nn::Sequential{torch::nn::Linear{input_size, hidden_layer_size},
                                 torch::nn::ELU{},
                                 torch::nn::Dropout{DROPOUT_ZERO_PROBABILITY},

                                 torch::nn::Linear{hidden_layer_size, hidden_layer_size},
                                 torch::nn::ELU{},
                                 torch::nn::Dropout{DROPOUT_ZERO_PROBABILITY},

                                 torch::nn::Linear{hidden_layer_size, hidden_layer_size},
                                 torch::nn::ELU{},
                                 torch::nn::Dropout{DROPOUT_ZERO_PROBABILITY}};
}

torch::nn::Linear Network::make_evs_head(std::size_t hidden_layer_size, std::size_t num_player_roles)
{
    return torch::nn::Linear{hidden_layer_size, num_player_roles};
}

torch::nn::Sequential Network::make_common_policy_head(std::size_t hidden_layer_size)
{
    return torch::nn::Sequential{torch::nn::Linear{hidden_layer_size, hidden_layer_size}, torch::nn::ELU{},
                                 torch::nn::Dropout{DROPOUT_ZERO_PROBABILITY}};
}

torch::nn::Sequential Network::make_policy_head(std::size_t max_policy_size) const
{
    return torch::nn::Sequential{torch::nn::Linear{hidden_layer_size, max_policy_size},
                                 torch::nn::Softmax{SOFTMAX_DIMENSION}};
}

Model::Model(const propnet::Propnet &propnet, std::string_view game) : Model{propnet, game, Network{propnet}}
{
}

Model Model::load_most_recent(const propnet::Propnet &propnet, std::string_view game)
{
    std::optional<std::filesystem::directory_entry> most_recent_file{};
    auto path{get_models_path(game)};
    for (const auto &file : std::filesystem::directory_iterator(path))
    {
        const auto is_model{file.path().extension() == MODEL_EXTENSION};
        const auto is_newer{!most_recent_file.has_value() ||
                            most_recent_file->last_write_time() < file.last_write_time()};
        if (is_model && is_newer)
        {
            most_recent_file.emplace(file);
        }
    }

    if (most_recent_file.has_value())
    {
        return load_file_path(propnet, game, most_recent_file->path());
    }
    else
    {
        throw std::runtime_error{"No recent model to be loaded"};
    }
}

Model Model::load_game_number(const propnet::Propnet &propnet, std::string_view game, std::size_t game_number)
{
    const auto file_name{get_file_name(game_number)};
    auto path{get_models_path(game)};
    path.append(file_name);

    return load_file_path(propnet, game, path);
}

ExpectedValue Model::eval_ev(const propnet::State &state, propnet::Role::Id id)
{
    auto evs{eval(state).evs};

    return evs[id].item<ExpectedValue>();
}

std::vector<ExpectedValue> Model::eval_evs(const propnet::State &state)
{
    auto evs{eval(state).evs};
    evs = evs.to(torch::kCPU);

    const auto start_ptr{evs.data_ptr<ExpectedValue>()};
    const auto end_ptr{start_ptr + evs.numel()};

    return std::vector<ExpectedValue>(start_ptr, end_ptr);
}

std::vector<Probability> Model::eval_policy(const propnet::State &state, propnet::Role::Id id)
{
    auto policy{eval(state).policies[id]};
    policy = policy.to(torch::kCPU);

    const auto start_ptr{policy.data_ptr<Probability>()};
    const auto end_ptr{start_ptr + policy.numel()};

    return std::vector<ExpectedValue>(start_ptr, end_ptr);
}

std::vector<std::vector<Probability>> Model::eval_policies(const propnet::State &state)
{
    auto policies_eval{eval(state).policies};
    policies_eval = policies_eval.to(torch::kCPU);

    std::vector<std::vector<Probability>> policies{};
    for (auto id{0u}; policies_eval.numel(); ++id)
    {
        const auto &policy{policies_eval[id]};
        const auto start_ptr{policy.data_ptr<Probability>()};
        const auto end_ptr{start_ptr + policy.numel()};
        policies.emplace_back(start_ptr, end_ptr);
    }

    return policies;
}

void Model::train(const ReplayBuffer &replay_buffer, std::size_t batch_size, std::size_t num_epochs)
{
    if (replay_buffer.size() <= batch_size)
    {
        return;
    }

    enable_training();
    cache->clear();

    double total_loss{0};
    for (std::size_t epoch_count{1}; epoch_count <= num_epochs; ++epoch_count)
    {
        auto sample{replay_buffer.sample(batch_size)};

        optimiser.zero_grad();

        auto output{network.forward(sample.states)};

        const auto evs_loss{loss_calculator->forward(output.evs, sample.evs)};
        evs_loss.backward();
        /*
        TODO: Currently the policies part of the network isn't being used for anything, so isn't trained

        const auto policies_loss{loss_calculator->forward(output.policies, sample.policies)};

        TODO: Come up with the weights appropriately depending on what needs to be optimised.
        Likely the evs are far more important that the policies.
        const auto loss{EVS_LOSS_WEIGHT * evs_loss + POLICIES_LOSS_WEIGHT * policies_loss};
        */
        const auto loss_scalar{evs_loss.item<double>()};
        total_loss += loss_scalar;

        optimiser.step();

        std::cout << "Epoch " << epoch_count << " loss: " << loss_scalar << '\n';
    }

    std::cout << "Total loss across epochs: " << total_loss << "\n\n";
    enable_eval();
}

void Model::save(std::size_t game_number)
{
    const auto file_name{get_file_name(game_number)};

    auto file_path{models_path};
    file_path.append(file_name);

    torch::serialize::OutputArchive output_archive{};
    network.save(output_archive);
    output_archive.save_to(file_path);

    const auto optimiser_name{get_optimiser_name(game_number)};
    auto optimiser_path{models_path};
    optimiser_path.append(optimiser_name);
    torch::save(optimiser, optimiser_path);

    log_time(game_number);

    std::cout << "Saved model to " << file_path << ", optimiser to " << optimiser_path << '\n';
}

std::filesystem::path Model::get_models_path(std::string_view game)
{
    auto path{std::filesystem::current_path()};
    path.append(MODELS_FOLDER_NAME);
    create_directory_if_not_exists(path);

    path.append(game);
    create_directory_if_not_exists(path);

    return path;
}

Model::Model(const propnet::Propnet &propnet, std::string_view game, Network &&network)
    : propnet{propnet}, models_path{get_models_path(game)}, network{std::move(network)},
      optimiser{this->network.parameters()}, loss_calculator{}, time_log_file{get_time_log_file_path(models_path)},
      start_time_ms{get_time_ms()}, cache{std::make_shared<Cache>()}
{
    this->network.to(device, dtype);
    std::cout << "Model loaded onto device '" << device << "' with dtype '" << dtype << "'\n";

    this->network.eval();
    std::cout << "Model created in evaluation mode (default)\n";

    loss_calculator->to(device, dtype);
}

void Model::enable_training()
{
    if (network.is_training())
    {
        throw std::logic_error{"Tried to enable training mode whilst already being in training mode"};
    }

    network.train();
}

void Model::enable_eval()
{
    if (!network.is_training())
    {
        throw std::logic_error{"Tried to enable evaluation mode whilst already being in evaluation mode"};
    }

    network.eval();
}

Network::Eval Model::eval(const propnet::State &state)
{
    if (const auto [eval, is_present]{cache->TryGet(state)}; is_present)
    {
        return *eval;
    }

    auto state_vec{state.to_vec<double>()};
    auto state_tensor{torch::from_blob(state_vec.data(), {static_cast<std::int64_t>(state_vec.size())},
                                       torch::TensorOptions().dtype(dtype))};
    state_tensor = state_tensor.to(device);
    auto eval{network.forward(std::move(state_tensor))};

    cache->Put(state, eval);

    return eval;
}

void Model::log_time(std::size_t game_number)
{
    const auto now_time_ms{get_time_ms()};
    const auto duration_ms{now_time_ms - start_time_ms};
    const auto num_ms{duration_ms.count()};
    const auto num_s{num_ms / 1e3};

    time_log_file << "Game number " << std::setfill('0') << std::setw(GAME_NUMBER_WIDTH) << game_number;
    time_log_file << " took " << num_s << " seconds to reach" << std::endl;
}

std::string Model::get_file_name(std::size_t game_number)
{
    std::stringstream file_name_stream{};
    file_name_stream << MODEL_NAME_BASE << std::setfill('0') << std::setw(GAME_NUMBER_WIDTH) << game_number
                     << MODEL_EXTENSION;

    return file_name_stream.str();
}

std::string Model::get_optimiser_name(std::size_t game_number)
{
    std::stringstream file_name_stream{};
    file_name_stream << OPTIMISER_NAME_BASE << std::setfill('0') << std::setw(GAME_NUMBER_WIDTH) << game_number
                     << MODEL_EXTENSION;

    return file_name_stream.str();
}

std::filesystem::path Model::get_time_log_file_path(std::filesystem::path models_path)
{
    models_path.append(TIME_LOG_FILE_NAME);

    return models_path;
}

void Model::create_directory_if_not_exists(const std::filesystem::path &path)
{
    if (!std::filesystem::exists(path))
    {
        std::filesystem::create_directory(path);
    }

    if (!std::filesystem::is_directory(path))
    {
        std::stringstream error_message{};
        error_message << "Path '" << path << "' already exists, but it's expected to be a directory";
        throw std::runtime_error{error_message.str()};
    }
}

std::chrono::milliseconds Model::get_time_ms()
{
    const auto now_time{std::chrono::system_clock::now()};
    const auto now_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_time.time_since_epoch());

    return now_time_ms;
}

Model Model::load_file_path(const propnet::Propnet &propnet, std::string_view game, const std::filesystem::path &path)
{
    // TODO: Load optimiser too
    Network network{propnet};
    torch::serialize::InputArchive input_archive{};
    input_archive.load_from(path);
    network.load(input_archive);

    return Model{propnet, game, std::move(network)};
}
} // namespace player
