#pragma once

#include "../../agents/include/agent.hpp"
#include "training_sampler.hpp"
#include "../include/cfr.hpp"

#include <iostream>
#include <concepts>
#include <mutex>
#include <thread>
#include <chrono>
#include <random>

namespace rebel
{
    static std::mt19937 random_engine {std::random_device {}()};

    template<typename DerivedSamplerT>
    concept DerivedSampler = requires(DerivedSamplerT sampler, const std::vector<bool>& observation, std::uint32_t prev_input)
    {
        { sampler.sample_state() } -> std::convertible_to<propnet::State>;
        { sampler.prepare_new_game() } -> std::convertible_to<void>;
        { sampler.add_history(observation, prev_input) } -> std::convertible_to<void>;
    };

    template<DerivedSampler SamplerT = TrainingSampler>
    class RebelAgent : public agents::Agent
    {
        private:
            static constexpr std::uint32_t DEFAULT_NUM_THREADS {6};
            static constexpr std::uint32_t MAX_SAMPLE_SIZE {200};
            static constexpr std::uint32_t MAX_CFR_TIME_SECONDS {10};

            SamplerT sampler;
            const propnet::Role& role;
            const propnet::Propnet& propnet;
            std::uint32_t num_threads; // TODO: Should this just be templated?
        public:
            RebelAgent(const propnet::Role& role, const propnet::Propnet& propnet, std::uint32_t num_threads) :
                Agent {role},
                sampler {role, propnet},
                role {role},
                propnet {propnet},
                num_threads {num_threads}
            {}

            RebelAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
                RebelAgent {role, propnet, DEFAULT_NUM_THREADS}
            {}

            static constexpr auto NAME {"rebel"};

            void prepare_new_game() override
            {
                Agent::prepare_new_game();
                sampler.prepare_new_game();
            }

            void add_history(std::uint32_t prev_input) override
            {
                const auto observations {get_observations_cache()};
                sampler.add_history(observations, prev_input);
            }

            std::uint32_t get_legal_input_impl(std::span<const std::uint32_t> legal_inputs)
            {
                std::vector<std::thread> threads {};

                auto num_sampled {0};
                std::mutex num_sampled_lock {};

                std::vector<double> cumulative_policy (legal_inputs.size(), 0.0);
                std::mutex cumulative_policy_lock {};

                const auto start_time {std::chrono::system_clock::now()};
                const auto end_time {start_time + std::chrono::seconds(MAX_CFR_TIME_SECONDS)};
                for (std::uint32_t thread_count {0}; thread_count < num_threads; ++thread_count)
                {
                    threads.emplace_back(
                        [this, &legal_inputs, &num_sampled, &num_sampled_lock, &cumulative_policy, &cumulative_policy_lock, end_time]
                        {
                            while (true)
                            {
                                const auto curr_time {std::chrono::system_clock::now()};
                                if (curr_time >= end_time)
                                {
                                    return;
                                }

                                {
                                    const std::lock_guard<std::mutex> num_sampled_guard {num_sampled_lock};
                                    if (num_sampled == MAX_SAMPLE_SIZE)
                                    {
                                        // Sample is full, stop
                                        return;
                                    }

                                    ++num_sampled;
                                }

                                // TODO
                                // const auto state {sampler.sample_state()};
                                // VanillaCfr cfr {role, propnet, legal_inputs, std::move(state)};
                                // const auto policy {cfr.search()};

                                // {
                                //     const std::lock_guard<std::mutex> cumulative_policy_guard {cumulative_policy_lock};
                                //     std::transform(
                                //         cumulative_policy.begin(),
                                //         cumulative_policy.end(),
                                //         policy.begin(),
                                //         cumulative_policy.begin(),
                                //         std::plus<double> {}
                                //     );
                                // }
                            }
                        }
                    );
                }

                std::for_each(
                    threads.begin(),
                    threads.end(),
                    [](auto& thread)
                    {
                        thread.join();
                    }
                );

                std::uniform_real_distribution<double> policy_distribution (0.0, num_sampled);
                const auto referee_selection {policy_distribution(random_engine)};
                auto accumulation {0.0};
                for (auto it {cumulative_policy.begin()}; it != cumulative_policy.end(); ++it)
                {
                    accumulation += *it;
                    if (accumulation > referee_selection)
                    {
                        const auto input_number {std::distance(cumulative_policy.begin(), it)};
                        return legal_inputs[input_number];
                    }
                }

                throw std::logic_error {"Cumulative policy exceeded the referees chosen number"};
            }
    };
}
