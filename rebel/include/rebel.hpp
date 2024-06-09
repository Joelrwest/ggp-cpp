#pragma once

#include "../../agents/include/agent.hpp"
#include "../src/sampler.hpp"
#include "training_sampler.hpp"
#include "../src/cfr.hpp"

#include <iostream>
#include <concepts>
#include <mutex>
#include <thread>
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
    } && std::derived_from<DerivedSamplerT, Sampler>;

    template<DerivedSampler SamplerT = TrainingSampler>
    class RebelAgent : public agents::Agent
    {
        private:
            static constexpr std::uint32_t NUM_THREADS {6};
            static constexpr std::uint32_t SAMPLE_SIZE {20};

            SamplerT sampler;
            const propnet::Role& role;
            Cfr cfr;

            // TODO: Only temp for testing, copied from human
            std::uint32_t temp_input_getter(std::span<const std::uint32_t> legal_inputs)
            {
                while (true)
                {
                    const auto& observations {get_observations_cache()};
                    role.print_observations(observations);
                    role.print_legal_inputs(legal_inputs);

                    std::uint32_t chosen;
                    std::cout << "Select a move by entering an integer: ";
                    std::cin >> chosen;
                    std::cout << "You've chosen " << chosen << '\n';

                    if (chosen < legal_inputs.size())
                    {
                        return legal_inputs[chosen];
                    }

                    std::cout << chosen << " was not found as an option, try again\n";
                }
            }
        public:
            RebelAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
                Agent {role},
                sampler {role, propnet},
                role {role},
                cfr {propnet}
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
                (void)legal_inputs;

                std::vector<std::thread> threads {};

                auto remaining_samples {SAMPLE_SIZE};
                std::mutex remaining_samples_lock {};

                std::vector<double> cum_policy (legal_inputs.size(), 0.0);
                std::mutex cum_policy_lock {};
                for (std::uint32_t thread_count {0}; thread_count < NUM_THREADS; ++thread_count)
                {
                    threads.emplace_back(
                        [this, &legal_inputs, &remaining_samples, &remaining_samples_lock, &cum_policy, &cum_policy_lock]
                        {
                            while (true)
                            {
                                {
                                    const std::lock_guard<std::mutex> remaining_samples_guard {remaining_samples_lock};
                                    if (remaining_samples == 0)
                                    {
                                        // Sample is full, stop
                                        return;
                                    }

                                    --remaining_samples;
                                }

                                const auto state {sampler.sample_state()};
                                const auto policy {cfr.search(legal_inputs, state)};

                                {
                                    const std::lock_guard<std::mutex> cum_policy_guard {cum_policy_lock};
                                    std::transform(
                                        cum_policy.begin(),
                                        cum_policy.end(),
                                        policy.begin(),
                                        cum_policy.begin(),
                                        std::plus<double> {}
                                    );
                                }
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

                std::uniform_real_distribution<double> policy_distribution (0.0, SAMPLE_SIZE);
                const auto referee_selection {policy_distribution(random_engine)};
                auto accumulation {0.0};
                for (auto it {cum_policy.begin()}; it != cum_policy.end(); ++it)
                {
                    accumulation += *it;
                    if (accumulation > referee_selection)
                    {
                        const auto input_number {std::distance(cum_policy.begin(), it)};
                        return legal_inputs[input_number];
                    }
                }

                // const auto input {temp_input_getter(legal_inputs)}; // TODO: Placeholder until rebel decides its own moves
                // return input;

                // TODO: This will be thrown later
                throw std::logic_error {"Cumulative policy exceeded the referees chosen number"};
            }
    };
}
