#pragma once

#include "../../propnet/include/propnet.hpp"
#include "misc.hpp"
#include "types.hpp"
#include <lru_cache_policy.hpp>

#include <torch/torch.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <string_view>
#include <utility>

namespace player
{
class ReplayBuffer
{
  public:
    ReplayBuffer(const propnet::Propnet &propnet);

    struct Item
    {
        Item(propnet::State state, std::vector<Policy> policies, std::vector<ExpectedValue> evs);

        std::vector<double> state;
        std::vector<Policy> policies;
        std::vector<ExpectedValue> evs;
    };

    struct Sample
    {
        torch::Tensor states;
        torch::Tensor policies;
        torch::Tensor evs;
    };

    template <typename... ItemArgs> void add(ItemArgs... item_args);
    Sample sample(std::size_t sample_size) const;
    std::size_t size() const;

  private:
    static constexpr auto MAX_SIZE{1500};

    const propnet::Propnet &propnet;
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
    struct Eval
    {
        torch::Tensor evs;
        torch::Tensor policies;
    };

    Network(const propnet::Propnet &propnet);
    Network(const Network &) = default;
    Network(Network &&) = default;

    Network &operator=(const Network &) = default;
    Network &operator=(Network &&) = default;

    Eval forward(torch::Tensor x);
    void piecewise_save(const std::filesystem::path &path)
        const; // TODO: Using torch::save/torch::load on Network directly doesn't work... not sure why
    void piecewise_load(const std::filesystem::path &path);

  private:
    static constexpr auto DROPOUT_ZERO_PROBABILITY{0.2};
    static constexpr auto SOFTMAX_DIMENSION{-1};

    static constexpr auto FEATURES_HEAD_NAME{"features"};
    static constexpr auto EVS_HEAD_NAME{"evs"};
    static constexpr auto COMMON_POLICY_HEAD_NAME{"common-policy"};
    static constexpr auto POLICY_HEAD_PREFIX{"policy-"};

    static torch::nn::Sequential make_features_head(std::size_t input_size, std::size_t hidden_layer_size);
    static torch::nn::Sequential make_evs_head(std::size_t hidden_layer_size, std::size_t num_player_roles);
    static torch::nn::Sequential make_common_policy_head(std::size_t hidden_layer_size);
    torch::nn::Sequential make_policy_head(std::size_t max_policy_size) const;

    std::size_t input_size;
    std::size_t hidden_layer_size;
    torch::nn::Sequential features_head;
    torch::nn::Sequential evs_head;
    torch::nn::Sequential common_policy_head;
    std::vector<torch::nn::Sequential> policy_heads;
};

class Model
{
  public:
    Model(const propnet::Propnet &propnet, std::string_view game);
    Model(const Model &) = default;
    Model(Model &&) = default;

    Model &operator=(const Model &) = default;
    Model &operator=(Model &&) = default;

    static constexpr auto MODELS_FOLDER_NAME{"models"};
    static constexpr auto TIME_LOG_FILE_NAME{"time-log.txt"};

    static Model load_game_number(const propnet::Propnet &propnet, std::string_view game, std::size_t game_number);

    void enable_training();
    void enable_eval();
    ExpectedValue eval_ev(const propnet::State &state, propnet::Role::Id id);
    std::vector<ExpectedValue> eval_evs(const propnet::State &state);
    std::vector<Probability> eval_policy(const propnet::State &state, propnet::Role::Id id);
    std::vector<std::vector<Probability>> eval_policies(const propnet::State &state);

    void train(const ReplayBuffer &replay_buffer);
    void save(std::size_t game_number);

    static std::filesystem::path get_models_path(std::string_view game);

  private:
    static constexpr auto MODEL_NAME_BASE{"game-num-"};
    static constexpr auto GAME_NUMBER_WIDTH{6};
    static constexpr auto MODEL_CACHE_SIZE{static_cast<std::size_t>(1e5)};
    static constexpr auto MODEL_NAME_EXTENSION{".pt"};
    static constexpr std::size_t BATCH_SIZE{64};
    static constexpr std::size_t NUM_EPOCHS{5};

    using Cache = misc::Cache<propnet::State, Network::Eval, caches::LRUCachePolicy, MODEL_CACHE_SIZE>;

    Model(const propnet::Propnet &propnet, std::string_view game, Network &&network);

    Network::Eval eval(const propnet::State &state);
    void log_time(std::size_t game_number);

    static std::string get_folder_name(std::size_t game_number);
    static std::filesystem::path get_time_log_file_path(std::filesystem::path models_path);
    static void create_directory_if_not_exists(const std::filesystem::path &path);
    static std::chrono::milliseconds get_time_ms();
    static Model load_folder_path(const propnet::Propnet &propnet, std::string_view game,
                                  const std::filesystem::path &file_name);

    const propnet::Propnet &propnet;
    std::filesystem::path models_path;
    Network network;
    torch::optim::Adam optimiser;
    torch::nn::MSELoss loss_calculator;
    std::ofstream time_log_file;
    std::chrono::milliseconds start_time_ms;
    std::shared_ptr<Cache> cache;
};
} // namespace player
