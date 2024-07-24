#pragma once

#include "../../agents/include/agent.hpp"
#include "../include/search.hpp"
#include "random_sampler.hpp"
#include "types.hpp"

#include <chrono>
#include <cmath>
#include <concepts>
#include <mutex>
#include <random>
#include <thread>

namespace player
{
template <typename DerivedSamplerT>
concept DerivedSampler = requires(DerivedSamplerT sampler, const std::vector<bool> &observation,
                                  propnet::PropId prev_input)
{
    {
        sampler.sample_state()
        } -> std::convertible_to<propnet::State>;
    {
        sampler.prepare_new_game()
        } -> std::convertible_to<void>;
    {
        sampler.add_history(observation, prev_input)
        } -> std::convertible_to<void>;
}; // TODO: Concept fails but idk why... && std::copyable<DerivedSamplerT>;

template <DerivedSampler SamplerT = RandomSampler> class Player : public agents::Agent
{
  public:
    Player(const propnet::Role &role, const propnet::Propnet &propnet, Model &model, std::size_t num_threads);
    Player(const propnet::Role &role, const propnet::Propnet &propnet, Model &model);
    Player(const Player &other) = default;
    Player(Player &&other) = default;

    Player &operator=(const Player &other) = default;
    Player &operator=(Player &&other) = default;

    void prepare_new_game() override;
    propnet::PropId get_legal_input_impl(std::span<const propnet::PropId> legal_inputs);

  protected:
    void add_history(propnet::PropId prev_input) override;

  private:
    static constexpr auto LOG_MAX_STATES_SEARCHED{std::log(1e15)};
    static constexpr std::size_t DEFAULT_NUM_THREADS{1};
    static constexpr std::size_t MAX_SAMPLE_SIZE{50};
    static constexpr auto MAX_CFR_ITERATIONS{static_cast<std::size_t>(1e5)};
    static constexpr std::chrono::seconds MAX_CFR_TIME_S{6};
    static constexpr std::chrono::seconds MAX_SEARCH_TIME_S{20};

    static Depth search_depth_limit_heuristic(const propnet::Propnet &propnet);

    SamplerT sampler;
    const propnet::Role &role; // TODO: Should be proper role not reference? Other spots too
    const propnet::Propnet &propnet;
    std::size_t num_threads;
    Depth search_depth_limit;
    Model &model;
};

template <DerivedSampler SamplerT>
Player<SamplerT>::Player(const propnet::Role &role, const propnet::Propnet &propnet, Model &model,
                         std::size_t num_threads)
    : Agent{role}, sampler{role, propnet}, role{role}, propnet{propnet}, num_threads{num_threads},
      search_depth_limit{search_depth_limit_heuristic(propnet)}, model{model}
{
}

template <DerivedSampler SamplerT>
Player<SamplerT>::Player(const propnet::Role &role, const propnet::Propnet &propnet, Model &model)
    : Player{role, propnet, model, DEFAULT_NUM_THREADS}
{
}

template <DerivedSampler SamplerT> void Player<SamplerT>::prepare_new_game()
{
    Agent::prepare_new_game();
    sampler.prepare_new_game();
}

template <DerivedSampler SamplerT>
propnet::PropId Player<SamplerT>::get_legal_input_impl(std::span<const propnet::PropId> legal_inputs)
{
    static std::mt19937 random_engine{std::random_device{}()};
    std::vector<std::thread> threads{};

    std::uint16_t num_sampled{0};
    std::mutex num_sampled_lock{};

    auto cumulative_policy{misc::make_zeroed_map<Policy>(legal_inputs)};
    std::mutex cumulative_policy_lock{};

    const auto start_time{std::chrono::system_clock::now()};
    const auto end_time{start_time + MAX_SEARCH_TIME_S};
    for (std::size_t thread_count{0}; thread_count < num_threads; ++thread_count)
    {
        threads.emplace_back([this, &legal_inputs, &num_sampled, &num_sampled_lock, &cumulative_policy,
                              &cumulative_policy_lock, end_time] {
            while (true)
            {
                const auto curr_time{std::chrono::system_clock::now()};
                if (curr_time >= end_time)
                {
                    return;
                }

                {
                    const std::lock_guard<std::mutex> num_sampled_guard{num_sampled_lock};
                    if (num_sampled == MAX_SAMPLE_SIZE)
                    {
                        return;
                    }

                    ++num_sampled;
                }

                search::DepthLimitedMCCFR mccfr{propnet, model, search_depth_limit};

                static auto options{player::search::MCCFR::Options{}
                                        .add_iteration_limit(MAX_CFR_ITERATIONS)
                                        .add_time_limit(MAX_CFR_TIME_S)};
                const auto search_results{mccfr.search(sampler.sample_state(), options)};

                const auto id{role.get_id()};
                const auto &role_search_results{search_results.at(id)};
                const auto &policy{role_search_results.first};

                {
                    const std::lock_guard<std::mutex> cumulative_policy_guard{cumulative_policy_lock};
                    for (auto &[input, probability] : cumulative_policy)
                    {
                        probability += policy.at(input);
                    }
                }
            }
        });
    }

    std::for_each(threads.begin(), threads.end(), [](auto &thread) { thread.join(); });

    return misc::sample_policy(cumulative_policy, static_cast<Probability>(num_sampled));
}

template <DerivedSampler SamplerT> Depth Player<SamplerT>::search_depth_limit_heuristic(const propnet::Propnet &propnet)
{
    const auto &roles{propnet.get_player_roles()};
    const auto max_branching_factor{std::accumulate(
        roles.begin(), roles.end(), propnet.is_randomness() ? propnet.get_random_role()->get_max_policy_size() : 1,
        [](const auto &accumulation, const auto &role) { return accumulation * role.get_max_policy_size(); })};

    return LOG_MAX_STATES_SEARCHED / std::log(max_branching_factor);
}

template <DerivedSampler SamplerT> void Player<SamplerT>::add_history(propnet::PropId prev_input)
{
    const auto observations{get_observations_cache()};
    sampler.add_history(observations, prev_input);
}
} // namespace player
