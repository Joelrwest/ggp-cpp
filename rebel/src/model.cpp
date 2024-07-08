#include "model.hpp"

#include <torch/script.h>

namespace rebel
{
    ReplayBuffer::ReplayBuffer() :
        buffer {}
    {}

    ReplayBuffer::Item::Item(propnet::State state, std::vector<Policy> policies) :
        state {std::move(state)},
        policies {std::move(policies)}
    {}

    std::vector<ReplayBuffer::Item> ReplayBuffer::sample(std::size_t sample_size) const
    {
        std::vector<ReplayBuffer::Item> sample {};
        std::sample(
            buffer.begin(),
            buffer.end(),
            std::back_inserter(sample),
            sample_size,
            std::mt19937 {std::random_device {}()}
        );

        return sample;
    }

    Network::Network(const propnet::Propnet& propnet) :
        input_size {propnet.size()},
        hidden_layer_size {input_size},
        features_head {
            torch::nn::Linear {input_size, hidden_layer_size},
            torch::nn::ELU {},
            torch::nn::Dropout {DROPOUT_ZERO_PROBABILITY},

            torch::nn::Linear {hidden_layer_size, hidden_layer_size},
            torch::nn::ELU {},
            torch::nn::Dropout {DROPOUT_ZERO_PROBABILITY},

            torch::nn::Linear {hidden_layer_size, hidden_layer_size},
            torch::nn::ELU {},
            torch::nn::Dropout {DROPOUT_ZERO_PROBABILITY}
        },
        values_head {
            torch::nn::Linear {hidden_layer_size, propnet.num_player_roles()},
            torch::nn::Sigmoid {}
        },
        common_policy_head {
            torch::nn::Linear {hidden_layer_size, hidden_layer_size},
            torch::nn::ELU {},
            torch::nn::Dropout {DROPOUT_ZERO_PROBABILITY}
        },
        policy_heads {}
    {
        const auto& player_roles {propnet.get_player_roles()};
        for (const auto& player_role : player_roles)
        {
            policy_heads.emplace_back(
                torch::nn::Linear {hidden_layer_size, player_role.get_max_policy_size()},
                torch::nn::Softmax {SOFTMAX_DIMENSION}
            );
        }
    }

    std::pair<torch::Tensor, std::vector<torch::Tensor>> Network::forward(torch::Tensor x)
    {
        /*
        Extract common features
        */
        x = features_head->forward(x);

        /*
        Get values for each role
        */
        const auto values {values_head->forward(x)};

        /*
        Common extra layer before each role gets a policy
        */
        x = common_policy_head->forward(x);

        /*
        Get policies for each role
        */
        std::vector<torch::Tensor> policies {};
        for (auto& policy_head : policy_heads)
        {
            const auto policy {policy_head->forward(x)};
            policies.push_back(std::move(policy));
        }

        return std::make_pair(std::move(values), std::move(policies));
    }

    Model::Model(const propnet::Propnet& propnet, std::string_view game) :
        Model {propnet, game, Network {propnet}}
    {}

    Model Model::load_most_recent(const propnet::Propnet& propnet, std::string_view game)
    {
        std::optional<std::filesystem::directory_entry> most_recent_file {};
        auto path {get_models_path(game)};
        for (const auto& file : std::filesystem::directory_iterator(path))
        {
            const auto is_model {file.path().extension() == MODEL_NAME_EXTENSION};
            const auto is_newer {!most_recent_file.has_value() || most_recent_file->last_write_time() < file.last_write_time()};
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
            throw std::runtime_error {"Tried to load most recent model but there are no files to be loaded"};
        }
    }

    Model Model::load_game_number(const propnet::Propnet& propnet, std::string_view game, std::size_t game_number)
    {
        const auto file_name {get_file_name(game_number)};
        auto path {get_models_path(game)};
        path.append(file_name);

        return load_file_path(propnet, game, path);
    }

    void Model::eval() const {}

    void Model::save(std::size_t game_number) const
    {
        torch::serialize::OutputArchive output_archive {};
        network.save(output_archive);

        const auto file_name {get_file_name(game_number)};

        auto file_path {models_path};
        file_path.append(file_name);

        output_archive.save_to(file_path);

        std::cout << "Saved model " << file_path << '\n';
    }

    Model::Model(const propnet::Propnet& propnet, std::string_view game, Network&& network) :
        propnet {propnet},
        models_path {get_models_path(game)},
        network {network},
        time_log_file {get_time_log_file_path(models_path)},
        start_time_ms {get_time_ms()},
        cache {}
    {}

    std::filesystem::path Model::get_models_path(std::string_view game)
    {
        auto path {std::filesystem::current_path()};
        path.append(MODELS_FOLDER_NAME);
        create_directory_if_not_exists(path);

        path.append(game);
        create_directory_if_not_exists(path);

        return path;
    }

    std::string Model::get_file_name(std::size_t game_number)
    {
        std::stringstream file_name_stream {};
        file_name_stream << MODEL_NAME_BASE << std::setw(GAME_NUMBER_WIDTH) << game_number << MODEL_NAME_EXTENSION;
        std::string file_name {};
        file_name_stream >> file_name;

        return file_name;
    }

    std::filesystem::path Model::get_time_log_file_path(std::filesystem::path models_path)
    {
        models_path.append(TIME_LOG_FILE_NAME);

        return models_path;
    }

    void Model::create_directory_if_not_exists(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path))
        {
            std::filesystem::create_directory(path);
        }

        if (!std::filesystem::is_directory(path))
        {
            std::stringstream error_message {};
            error_message << "Path '" << path << "' already exists, but it's expected to be a directory";
            throw std::runtime_error {error_message.str()};
        }
    }

    std::chrono::milliseconds Model::get_time_ms()
    {
        const auto now_time {std::chrono::system_clock::now()};
        const auto now_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_time.time_since_epoch());

        return now_time_ms;
    }

    Model Model::load_file_path(const propnet::Propnet& propnet, std::string_view game, const std::filesystem::path& path)
    {
        Network network {propnet};
        torch::serialize::InputArchive input_archive {};
        input_archive.load_from(path);
        network.load(input_archive);

        return Model {propnet, game, std::move(network)};
    }

    void Model::log_time(std::size_t game_number)
    {
        const auto now_time_ms {get_time_ms()};
        const auto duration_ms {now_time_ms - start_time_ms};
        const auto num_ms {duration_ms.count()};
        const auto num_s {num_ms / 10e3};

        time_log_file << "Game number " << game_number << " took " << num_s << " seconds to reach\n";
    }
}
