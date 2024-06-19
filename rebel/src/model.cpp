#include "model.hpp"

#include <iostream>

namespace rebel
{
    /*
    def __init__(self, num_inputs, num_actions, roles, head_depth=3):
        super().__init__()
        self.dropout = nn.Dropout(p=0.2)
        self.activ = nn.ELU()
        self.sigmoid = nn.Sigmoid()
        self.softmax = nn.Softmax(-1)
        self.head = nn.Sequential(*[
            nn.Sequential(nn.Linear(num_inputs, num_inputs), self.activ, self.dropout)
        for _ in range(head_depth)])
        self.val = nn.Linear(num_inputs, len(roles))
        self.policy_head = nn.Sequential(nn.Linear(num_inputs, num_inputs), self.activ, self.dropout)
        self.policy = nn.Sequential(*[nn.Linear(num_inputs, num_actions[role]) for role in roles])
        self.max_actions = max(num_actions[role] for role in roles)

    def forward(self, x):
        x = self.head(x)

        val = self.sigmoid(self.val(x))
        x = self.policy_head(x)
        if len(x.shape) > 1:
            policies = torch.zeros((x.shape[0], val.shape[-1], self.max_actions))
            for i, policy_layer in enumerate(self.policy):
                out = self.softmax(policy_layer(x))
                policies[:,i, :out.shape[-1]] = out
        else:
            policies = [self.softmax(policy_layer(x)) for policy_layer in self.policy]

        return policies, val
    */

    Network::Network(const propnet::Propnet& propnet) :
        linear_layers {},
        sigmoid {},
        softmax {SOFTMAX_DIMENSION},
        max_policy_sizes {}
    {
        const torch::nn::Dropout dropout {DROPOUT_ZERO_PROBABILITY};
        const torch::nn::ELU activation {};

        const auto& roles {propnet.get_roles()};
        std::transform(
            roles.begin(),
            roles.end(),
            std::back_inserter(max_policy_sizes),
            [](const auto& role)
            {
                return role.get_max_policy_size();
            }
        );
    }

    torch::Tensor Network::forward(torch::Tensor inputs) const
    {
        // // Use one of many tensor manipulation functions.
        // x = torch::relu(fc1->forward(x.reshape({x.size(0), 784})));
        // x = torch::dropout(x, /*p=*/0.5, /*train=*/is_training());
        // x = torch::relu(fc2->forward(x));
        // x = torch::log_softmax(fc3->forward(x), /*dim=*/1);
        return inputs;
    }

    /*
    def load(self, path):
        try:
            self.network.load_state_dict(torch.load(path))
            logging.info(f"Loaded {path}")
        except Exception as error:
            logging.error(error)
            exit(1)

    def load_most_recent(self):
        models_folder_path = os.path.join(pathlib.Path(__file__).parent.parent, 'models')
        game_model_folder_path = os.path.join(models_folder_path, self.game_str)

        newest_model_path = max(os.listdir(game_model_folder_path))
        self.load(os.path.join(game_model_folder_path, newest_model_path))
    */

    // Model Model::load_most_recent(const propnet::Propnet& propnet, std::string_view game) { (void)propnet, game; }
    // Model Model::load_game_number(const propnet::Propnet& propnet, std::string_view game, int game_number) { (void)propnet, game, game_number; }

    void Model::eval() const {}

    void Model::save(int game_number)
    {
        torch::serialize::OutputArchive output_archive {};
        network.save(output_archive);

        std::stringstream file_name_stream {};
        file_name_stream << MODEL_NAME_PREFIX << std::setw(GAME_NUMBER_WIDTH) << game_number << MODEL_NAME_POSTFIX;
        std::string file_name {};
        file_name_stream >> file_name;

        auto file_path {models_path};
        file_path.append(file_name);

        output_archive.save_to(file_path);

        log_time(game_number);

        std::cout << "Saved model " << file_path << '\n';
    }

    Model::Model(const propnet::Propnet& propnet, std::string_view game, Network&& network) :
        propnet {propnet},
        models_path {get_models_path(game)},
        network {network},
        time_log_file {get_time_log_file_path(models_path)},
        start_time_ms {get_time_ms()}
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

    void Model::log_time(int game_number)
    {
        const auto now_time_ms {get_time_ms()};
        const auto duration_ms {now_time_ms - start_time_ms};
        const auto num_ms {duration_ms.count()};
        const auto num_s {num_ms / 10e3};

        time_log_file << "Game number " << game_number << " took " << num_s << " seconds to reach\n";
    }
}
