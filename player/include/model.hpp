#pragma once

#include "../../propnet/include/propnet.hpp"
#include "misc.hpp"
#include "types.hpp"
#include <lru_cache_policy.hpp>

#include <torch/torch.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <utility>

namespace player
{
class ReplayBuffer
{
  public:
    ReplayBuffer();

    struct Item
    {
        Item(propnet::State state, std::vector<Policy> policies);

        propnet::State state;
        std::vector<Policy> policies;
        // std::vector<ExpectedValue> values; // TODO: Add
    };

    template <typename... ItemArgs> void add(ItemArgs... item_args);

    std::vector<Item> sample(std::size_t sample_size) const;

  private:
    static constexpr auto MAX_SIZE{2500};

    std::deque<Item> buffer;
};

template <typename... ItemArgs> void ReplayBuffer::add(ItemArgs... item_args)
{
    buffer.emplace_back(item_args...);

    if (buffer.size() > MAX_SIZE)
    {
        buffer.pop_front();
    }
}

class Network : public torch::nn::Module
{
  public:
    using Eval = std::pair<torch::Tensor, std::vector<torch::Tensor>>;

    Network(const propnet::Propnet &propnet);

    Eval forward(torch::Tensor x);

  private:
    static constexpr auto DROPOUT_ZERO_PROBABILITY{0.2};
    static constexpr auto SOFTMAX_DIMENSION{1}; // TODO: Not 100% sure about this

    std::size_t input_size;
    std::size_t hidden_layer_size;
    torch::nn::Sequential features_head;
    torch::nn::Sequential values_head;
    torch::nn::Sequential common_policy_head;
    std::vector<torch::nn::Sequential> policy_heads;
};

class Model
{
  public:
    Model(const propnet::Propnet &propnet, std::string_view game);

    static constexpr auto MODELS_FOLDER_NAME{"models"};
    static constexpr auto TIME_LOG_FILE_NAME{"time-log.txt"};

    static Model load_most_recent(const propnet::Propnet &propnet, std::string_view game);
    static Model load_game_number(const propnet::Propnet &propnet, std::string_view game, std::size_t game_number);

    void eval() const; // TODO: What *exactly* do I want to take in and give back?
    void save(std::size_t game_number) const;

  private:
    static constexpr auto MODEL_NAME_BASE{"game-num-"};
    static constexpr auto GAME_NUMBER_WIDTH{6};
    static constexpr auto MODEL_CACHE_SIZE{10000};
    static constexpr auto MODEL_NAME_EXTENSION{".ckpt"};

    Model(const propnet::Propnet &propnet, std::string_view game, Network &&network);

    static std::filesystem::path get_models_path(std::string_view game);
    static std::string get_file_name(std::size_t game_number);
    static std::filesystem::path get_time_log_file_path(std::filesystem::path models_path);
    static void create_directory_if_not_exists(const std::filesystem::path &path);
    static std::chrono::milliseconds get_time_ms();
    static Model load_file_path(const propnet::Propnet &propnet, std::string_view game,
                                const std::filesystem::path &file_name);

    void log_time(std::size_t game_number);

    const propnet::Propnet &propnet;
    std::filesystem::path models_path;
    Network network;
    std::ofstream time_log_file;
    std::chrono::milliseconds start_time_ms;
    misc::Cache<propnet::State, Network::Eval, caches::LRUCachePolicy, MODEL_CACHE_SIZE> cache;
};
} // namespace player
