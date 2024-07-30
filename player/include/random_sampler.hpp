#pragma once

#include "../../propnet/include/propnet.hpp"

#include <deque>
#include <shared_mutex>
#include <tuple>

namespace player
{
class RandomSampler
{
  public:
    RandomSampler(const propnet::Role &sampler_role, const propnet::Propnet &propnet);
    RandomSampler(const RandomSampler &other) = default;
    RandomSampler(RandomSampler &&other) = default;

    RandomSampler &operator=(const RandomSampler &other) = default;
    RandomSampler &operator=(RandomSampler &&other) = default;

    void prepare_new_game();
    void add_history(const std::vector<bool> &observation, propnet::PropId prev_input, std::span<const propnet::PropId> legal_inputs);
    void pop_history();
    propnet::State sample_state();
    std::vector<propnet::State> sample_states(std::size_t num_states);

  private:
    struct History
    {
        History(const std::vector<bool> &observation, propnet::PropId prev_input, std::span<const propnet::PropId> legal_inputs);
        History(const History &other);
        History(History &&other);

        History &operator=(const History &other);
        History &operator=(History &&other);

        std::vector<bool> observation;
        propnet::PropId prev_input;
        std::vector<propnet::PropId> legal_inputs;
        std::unordered_set<propnet::State> invalid_state_cache;
        std::shared_mutex invalid_state_cache_lock;
        std::unordered_set<propnet::InputSet> invalid_inputs_cache;
        std::shared_mutex invalid_inputs_cache_lock;
    };

    std::deque<History> all_histories{};
    const propnet::Propnet &propnet;
    propnet::Role sampler_role;
    std::vector<propnet::Role> player_roles;
    std::optional<propnet::Role> random_role;

    using AllHistories = decltype(all_histories);

    std::optional<propnet::State> sample_state_impl(AllHistories::iterator all_histories_it,
                                                    AllHistories::iterator all_histories_end_it, propnet::State state);
    inline static bool is_invalid_state(History& history, const propnet::State &state);
    inline static void add_invalid_state(History& history, const propnet::State &state);
    inline static bool is_invalid_inputs(History& history, const propnet::InputSet &inputs);
    inline static void add_invalid_inputs(History& history, const propnet::InputSet &inputs);
};
} // namespace player
