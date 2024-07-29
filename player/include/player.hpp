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
    class Options
    {
      public:
        Options();
        Options(const Options &other) = default;
        Options(Options &&other) = default;

        Options &operator=(const Options &other) = default;
        Options &operator=(Options &&other) = default;

        Options &add_thread_limit(std::size_t thread_limit);
        std::size_t get_thread_limit() const;

        Options &add_max_sample_size(std::size_t max_sample_size);
        std::size_t get_max_sample_size() const;

        template <typename T> Options &add_total_time_limit(T duration);
        std::function<bool()> get_total_time_remaining_function() const;

        template <typename T> Options &add_cfr_time_limit(T duration);
        Options &add_cfr_iteration_limit(std::size_t iteration_limit);
        search::MCCFR::Options &get_cfr_options();

      private:
        static constexpr std::size_t DEFAULT_THREAD_LIMIT{1};
        static constexpr std::size_t DEFAULT_MAX_SAMPLE_SIZE{50};
        static constexpr std::chrono::seconds DEFAULT_CFR_TIME_LIMIT_S{100};

        std::size_t thread_limit;
        std::size_t max_sample_size;
        misc::TimeOption total_time_limit_option;
        search::MCCFR::Options cfr_options;
    };

    Player(const propnet::Role &role, const propnet::Propnet &propnet, Model &model);
    Player(const propnet::Role &role, const propnet::Propnet &propnet, Model &model, Options options);
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

    static Depth search_depth_limit_heuristic(const propnet::Propnet &propnet);

    SamplerT sampler;
    const propnet::Role &role; // TODO: Should be proper role not reference? Other spots too
    const propnet::Propnet &propnet;
    Options options;
    Depth search_depth_limit;
    Model &model;
};

template <DerivedSampler SamplerT>
Player<SamplerT>::Options::Options()
    : thread_limit{DEFAULT_THREAD_LIMIT}, max_sample_size{DEFAULT_MAX_SAMPLE_SIZE}, total_time_limit_option{},
      cfr_options{search::MCCFR::Options{}.add_time_limit(DEFAULT_CFR_TIME_LIMIT_S)}
{
}

template <DerivedSampler SamplerT>
Player<SamplerT>::Options &Player<SamplerT>::Options::add_thread_limit(std::size_t thread_limit)
{
    this->thread_limit = thread_limit;
    return *this;
}

template <DerivedSampler SamplerT> std::size_t Player<SamplerT>::Options::get_thread_limit() const
{
    return thread_limit;
}

template <DerivedSampler SamplerT>
Player<SamplerT>::Options &Player<SamplerT>::Options::add_max_sample_size(std::size_t max_sample_size)
{
    this->max_sample_size = max_sample_size;
    return *this;
}

template <DerivedSampler SamplerT> std::size_t Player<SamplerT>::Options::get_max_sample_size() const
{
    return max_sample_size;
}

template <DerivedSampler SamplerT>
template <typename T>
Player<SamplerT>::Options &Player<SamplerT>::Options::add_total_time_limit(T duration)
{
    total_time_limit_option.add(duration);
    return *this;
}

template <DerivedSampler SamplerT>
std::function<bool()> Player<SamplerT>::Options::get_total_time_remaining_function() const
{
    return total_time_limit_option.get_function();
}

template <DerivedSampler SamplerT>
template <typename T>
Player<SamplerT>::Options &Player<SamplerT>::Options::add_cfr_time_limit(T duration)
{
    cfr_options.add_time_limit(duration);
    return *this;
}

template <DerivedSampler SamplerT>
Player<SamplerT>::Options &Player<SamplerT>::Options::add_cfr_iteration_limit(std::size_t iteration_limit)
{
    cfr_options.add_iteration_limit(iteration_limit);
    return *this;
}

template <DerivedSampler SamplerT> search::MCCFR::Options &Player<SamplerT>::Options::get_cfr_options()
{
    return cfr_options;
}

template <DerivedSampler SamplerT>
Player<SamplerT>::Player(const propnet::Role &role, const propnet::Propnet &propnet, Model &model)
    : Player{role, propnet, model, Options{}}
{
}

template <DerivedSampler SamplerT>
Player<SamplerT>::Player(const propnet::Role &role, const propnet::Propnet &propnet, Model &model, Options options)
    : Agent{role}, sampler{role, propnet}, role{role}, propnet{propnet}, options{std::move(options)},
      search_depth_limit{search_depth_limit_heuristic(propnet)}, model{model}
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
    const auto thread_limit{options.get_thread_limit()};
    for (std::size_t thread_count{0}; thread_count < thread_limit; ++thread_count)
    {
        threads.emplace_back(
            [this, &legal_inputs, &num_sampled, &num_sampled_lock, &cumulative_policy, &cumulative_policy_lock] {
                auto &cfr_options{options.get_cfr_options()};
                const auto total_time_remaining_function{options.get_total_time_remaining_function()};
                const auto max_sample_size{options.get_max_sample_size()};
                while (total_time_remaining_function())
                {
                    {
                        const std::lock_guard<std::mutex> num_sampled_guard{num_sampled_lock};
                        if (num_sampled == max_sample_size)
                        {
                            return;
                        }

                        ++num_sampled;
                    }

                    search::DepthLimitedMCCFR mccfr{propnet, model, search_depth_limit};

                    const auto search_results{mccfr.search(sampler.sample_state(), cfr_options)};

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
