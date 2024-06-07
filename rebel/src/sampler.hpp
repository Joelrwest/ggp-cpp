#pragma once

#include "../../propnet/include/vector_state.hpp"
#include "../../propnet/include/propnet.hpp"
#include "../../propnet/include/parser.hpp"
#include "../../agents/include/non_seeing.hpp" // TODO

#include <concepts>
#include <thread>
#include <optional>
#include <mutex>

namespace rebel
{
    template<typename DerivedSamplerT>
    class Sampler;

    template<typename DerivedSamplerT>
    concept DerivedSampler = requires(DerivedSamplerT derived_sampler, const std::vector<bool>& observation, std::uint32_t prev_input)
    {
        { derived_sampler.sample_state() } -> std::convertible_to<propnet::State>;
        { derived_sampler.prepare_new_game() } -> std::convertible_to<void>;
        { derived_sampler.add_history(observation, prev_input) } -> std::convertible_to<void>;
    } && std::derived_from<DerivedSamplerT, Sampler<DerivedSamplerT>>;

    template<typename DerivedSamplerT>
    class Sampler
    {
        private:
            static constexpr auto NUM_THREADS {6};
            static constexpr auto SAMPLE_SIZE {20};

            DerivedSamplerT& to_derived()
            {
                return static_cast<DerivedSamplerT&>(*this);
            }
        public:
            /*
            Assumes that calls to sample_state() for
            derived classes are thread safe!!
            Otherwise truly cursed undefined behaviour
            may happen.
            */
            std::vector<propnet::State> sample()
            {
                /*
                Start new games and play until we get
                the same sees as we've got here
                */
                std::vector<propnet::State> sample (SAMPLE_SIZE);
                std::vector<std::thread> threads {};
                auto sample_it {sample.begin()};
                std::mutex sample_it_lock {};
                for (auto thread_count {0}; thread_count < NUM_THREADS; ++thread_count)
                {
                    threads.emplace_back(
                        [this, &sample_it, &sample_it_lock, sample_end_it = sample.end()]
                        {
                            try {
                                while (true)
                                {
                                    /*
                                    Check if the sample is full, and if not
                                    allocate a slot to place the next sample.

                                    This is the only concurrency issue that can arise
                                    in terms of the vector, however the actual sampler is
                                    assumed to be safe to use concurrently too.
                                    */
                                    decltype(sample_it) inserting_it;
                                    {
                                        const std::lock_guard<std::mutex> sample_guard {sample_it_lock};
                                        if (sample_it == sample_end_it)
                                        {
                                            // Sample is full, stop
                                            return;
                                        }

                                        inserting_it = sample_it;
                                        ++sample_it;
                                    }

                                    const propnet::State sampled_state {to_derived().sample_state()};
                                    *inserting_it = std::move(sampled_state);
                                }
                            }
                            catch (const std::runtime_error& error)
                            {
                                std::cout << error.what() << '\n';
                                throw error;
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

                return sample;
            }

            template<typename AgentT, typename... AgentArgsT>
            static std::vector<AgentT> create_player_agents(const propnet::Role& sampler_role, const propnet::Propnet& propnet, AgentArgsT... agent_args)
            {
                std::vector<AgentT> agents {};
                const auto& all_roles {propnet.get_roles()};
                for (const auto& role : all_roles)
                {
                    const auto is_self_or_random {
                        role.get_name() == sampler_role.get_name() ||
                        role.get_name() == propnet::Parser::RANDOM_PLAYER_NAME
                    };

                    if (!is_self_or_random)
                    {
                        agents.emplace_back(role, agent_args...);
                    }
                }

                return agents;
            }

            static std::optional<agents::RandomAgent> try_create_random_agent(const propnet::Propnet& propnet)
            {
                const auto& all_roles {propnet.get_roles()};
                for (const auto& role : all_roles)
                {
                    if (role.get_name() == propnet::Parser::RANDOM_PLAYER_NAME)
                    {
                        return std::optional<agents::RandomAgent> {role};
                    }
                }

                return std::optional<agents::RandomAgent> {};
            }
    };
}
