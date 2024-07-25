#include "../include/model.hpp"

#include <torch/script.h>

namespace player
{
static const torch::Device device{torch::cuda::is_available() ? torch::kCUDA : torch::kCPU};

ReplayBuffer::ReplayBuffer(const propnet::Propnet &propnet) : propnet{propnet}, buffer{}
{
}

ReplayBuffer::Item::Item(propnet::State state, std::vector<Policy> policies, std::vector<ExpectedValue> evs)
    : state{std::move(state)}, policies{std::move(policies)}, evs{std::move(evs)}
{
}

ReplayBuffer::Sample ReplayBuffer::sample(std::size_t sample_size) const
{
    std::vector<ReplayBuffer::Item> sample_items{};
    std::sample(buffer.begin(), buffer.end(), std::back_inserter(sample_items), sample_size,
                std::mt19937{std::random_device{}()});

    std::vector<float> linear_states{};
    std::vector<float> linear_policies{};
    std::vector<float> linear_evs{};
    const auto max_policy_size{propnet.get_max_policy_size()};
    for (auto &sample_item : sample_items)
    {
        const auto state{sample_item.state.to_vec<float>()};
        std::copy(state.begin(), state.end(), std::back_inserter(linear_states));

        for (auto &policy : sample_item.policies)
        {
            std::transform(policy.begin(), policy.end(), std::back_inserter(linear_policies),
                           [](const auto &entry) { return entry.second; });
            const auto num_fill{max_policy_size - policy.size()};
            std::fill_n(std::back_inserter(linear_policies), num_fill, 0.0);
        }

        const auto &evs{sample_item.evs};
        std::copy(evs.begin(), evs.end(), std::back_inserter(linear_evs));
    }

    const auto num_players{propnet.num_player_roles()};
    auto states_tensor{torch::from_blob(
        linear_states.data(), {static_cast<std::int64_t>(sample_size), static_cast<std::int64_t>(propnet.size())},
        torch::TensorOptions().device(device).dtype(torch::kFloat32))};
    auto policies_tensor{
        torch::from_blob(linear_policies.data(),
                         {static_cast<std::int64_t>(sample_size), static_cast<std::int64_t>(num_players),
                          static_cast<std::int64_t>(max_policy_size)},
                         torch::TensorOptions().device(device).dtype(torch::kFloat64))};
    auto evs_tensor{torch::from_blob(linear_evs.data(),
                                     {static_cast<std::int64_t>(sample_size), static_cast<std::int64_t>(num_players)},
                                     torch::TensorOptions().device(device).dtype(torch::kFloat64))};

    return {.states = std::move(states_tensor), .policies = std::move(policies_tensor), .evs = std::move(evs_tensor)};
}

std::size_t ReplayBuffer::size() const
{
    return buffer.size();
}

Network::Network(const propnet::Propnet &propnet)
    : input_size{propnet.size()}, hidden_layer_size{input_size},
      features_head{register_module("features", make_features_head(input_size, hidden_layer_size))},
      evs_head{register_module("evs", make_evs_head(hidden_layer_size, propnet.num_player_roles()))},
      common_policy_head{register_module("common-policy", make_common_policy_head(hidden_layer_size))}, policy_heads{}
{
    const auto max_policy_size{propnet.get_max_policy_size()};
    for (auto head_count{0u}; head_count < propnet.num_player_roles(); ++head_count)
    {
        policy_heads.emplace_back(register_module("policy-" + head_count, make_policy_head(max_policy_size)));
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
    auto evs{evs_head->forward(x).to(torch::kFloat64)}; // TODO: See comment below

    /*
    Common extra layer before each role gets a policy
    */
    x = common_policy_head->forward(x);

    /*
    Get policies for each role
    */
    std::vector<torch::Tensor> policies{};
    for (auto &policy_head : policy_heads)
    {
        policies.push_back(policy_head->forward(x).to(
            torch::kFloat64)); // TODO: How to take in float64's instead so we don't need to do the conversion
    }

    return {.evs = std::move(evs), .policies = torch::stack(policies)};
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

torch::nn::Sequential Network::make_evs_head(std::size_t hidden_layer_size, std::size_t num_player_roles)
{
    return torch::nn::Sequential{torch::nn::Linear{hidden_layer_size, num_player_roles}, torch::nn::Sigmoid{}};
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
        const auto is_model{file.path().extension() == MODEL_NAME_EXTENSION};
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
void Model::enable_training()
{
    network.train();
}

void Model::disable_training()
{
    network.train(false);
}

ExpectedValue Model::eval_ev(const propnet::State &state, propnet::Role::Id id)
{
    auto evs{eval(state).evs};

    return evs[id].item<ExpectedValue>();
}

std::vector<ExpectedValue> Model::eval_evs(const propnet::State &state)
{
    auto evs{eval(state).evs};
    const auto start_ptr{evs.data_ptr<ExpectedValue>()};
    const auto end_ptr{start_ptr + evs.numel()};

    return std::vector<ExpectedValue>(start_ptr, end_ptr);
}

std::vector<Probability> Model::eval_policy(const propnet::State &state, propnet::Role::Id id)
{
    auto policy{eval(state).policies[id]};
    const auto start_ptr{policy.data_ptr<Probability>()};
    const auto end_ptr{start_ptr + policy.numel()};

    return std::vector<ExpectedValue>(start_ptr, end_ptr);
}

std::vector<std::vector<Probability>> Model::eval_policies(const propnet::State &state)
{
    const auto policies_eval{eval(state).policies};

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

void Model::train(const ReplayBuffer &replay_buffer)
{
    if (replay_buffer.size() <= BATCH_SIZE)
    {
        return;
    }

    torch::optim::Adam optimiser{network.parameters()};
    torch::nn::MSELoss loss_calculator{};
    double total_loss{0};

    cache->clear();
    for (std::size_t epoch_count{1}; epoch_count <= NUM_EPOCHS; ++epoch_count)
    {
        auto sample{replay_buffer.sample(BATCH_SIZE)};
        std::cout << "Took sample!\n";
        std::cout << sample.states << "\n\n\n";
        std::cout << sample.policies << "\n\n\n";
        std::cout << sample.evs << "\n\n\n";

        optimiser.zero_grad();

        auto output{network.forward(sample.states)};

        const auto evs_loss{loss_calculator->forward(output.evs, sample.evs)};
        const auto policies_loss{loss_calculator->forward(output.policies, sample.policies)};
        const auto loss{evs_loss + policies_loss};
        const auto loss_scalar{loss.item<double>()};
        total_loss += loss_scalar;

        loss.backward();
        optimiser.step();

        std::cout << "Epoch " << epoch_count << " loss: " << loss_scalar << '\n';
    }

    std::cout << "Total loss across epochs: " << total_loss << "\n\n";
}

void Model::save(std::size_t game_number)
{
    torch::serialize::OutputArchive output_archive{};
    network.save(output_archive);

    const auto file_name{get_file_name(game_number)};

    auto file_path{models_path};
    file_path.append(file_name);

    output_archive.save_to(file_path);

    log_time(game_number);

    std::cout << "Saved model to " << file_path << '\n';
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
    : propnet{propnet}, models_path{get_models_path(game)}, network{network},
      time_log_file{get_time_log_file_path(models_path)}, start_time_ms{get_time_ms()}, cache{std::make_shared<Cache>()}
{
    network.to(device);
}

Network::Eval Model::eval(const propnet::State &state)
{
    if (const auto [eval, is_present]{cache->TryGet(state)}; is_present)
    {
        return *eval;
    }

    auto vec{state.to_vec<float>()};
    auto tensor{torch::from_blob(vec.data(), {static_cast<std::int64_t>(vec.size())},
                                 torch::TensorOptions().device(device).dtype(torch::kFloat32))};
    auto eval{network.forward(std::move(tensor))};

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
    time_log_file << " took " << num_s << " seconds to reach\n";
    time_log_file.flush();
}

std::string Model::get_file_name(std::size_t game_number)
{
    std::stringstream file_name_stream{};
    file_name_stream << MODEL_NAME_BASE << std::setfill('0') << std::setw(GAME_NUMBER_WIDTH) << game_number
                     << MODEL_NAME_EXTENSION;

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
    Network network{propnet};
    torch::serialize::InputArchive input_archive{};
    input_archive.load_from(path);
    network.load(input_archive);

    return Model{propnet, game, std::move(network)};
}
} // namespace player
