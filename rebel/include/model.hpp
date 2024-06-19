#pragma once

#include "../../propnet/include/propnet.hpp"

#include <torch/torch.h>

#include <string_view>
#include <filesystem>
#include <fstream>
#include <chrono>

namespace rebel
{
    class Network : public torch::nn::Module
    {
        public:
            Network(const propnet::Propnet& propnet);

            torch::Tensor forward(torch::Tensor inputs) const;
        private:
            static constexpr auto NUM_LINEAR_LAYERS {3};
            static constexpr auto DROPOUT_ZERO_PROBABILITY {0.2};
            static constexpr auto SOFTMAX_DIMENSION {1}; // TODO: Not 100% sure about this

            torch::nn::Sequential linear_layers;
            torch::nn::Sigmoid sigmoid;
            torch::nn::Softmax softmax;
            std::vector<std::size_t> max_policy_sizes;
    };

    class Model
    {
        public:
            static constexpr auto MODELS_FOLDER_NAME {"models"};
            static constexpr auto TIME_LOG_FILE_NAME {"time-log.txt"};

            // static Model create_new(const propnet::Propnet& propnet, std::string_view game);
            // static Model load_most_recent(const propnet::Propnet& propnet, std::string_view game);
            // static Model load_game_number(const propnet::Propnet& propnet, std::string_view game, int game_number);

            void eval() const; // TODO: What *exactly* do I want to take in and give back?
            void save(int game_number);
        private:
            static constexpr auto MODEL_NAME_PREFIX {"game-num-"};
            static constexpr auto GAME_NUMBER_WIDTH {6};
            static constexpr auto MODEL_NAME_POSTFIX {".ckpt"};

            Model(const propnet::Propnet& propnet, std::string_view game, Network&& network);

            static std::filesystem::path get_models_path(std::string_view game);
            static std::filesystem::path get_time_log_file_path(std::filesystem::path models_path);
            static void create_directory_if_not_exists(const std::filesystem::path& path);
            static std::chrono::milliseconds get_time_ms();

            void log_time(int game_number);

            const propnet::Propnet& propnet;
            std::filesystem::path models_path;
            Network network;
            std::ofstream time_log_file;
            std::chrono::milliseconds start_time_ms;
    };
}
