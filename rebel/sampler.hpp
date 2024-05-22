#pragma once

#include "../propnet/include/vector_state.hpp"
#include "../propnet/include/propnet.hpp"
#include "../agents/include/simple.hpp"

#include <concepts>
#include <future>
#include <thread>

namespace rebel
{
    template<typename DerivedT>
    class Sampler
    {
        private:
            static constexpr auto NUM_THREADS {6};
            static constexpr auto PER_THREAD_SAMPLE_SIZE {3};
            static constexpr auto SAMPLE_SIZE {NUM_THREADS * PER_THREAD_SAMPLE_SIZE};

            DerivedT& to_derived()
            {
                return static_cast<DerivedT&>(*this);
            }

            std::vector<std::vector<bool>> all_sees {};
        public:
            const std::vector<std::vector<bool>>& get_all_sees() const
            {
                return all_sees;
            }

            void add_sees(const std::vector<bool>& sees)
            {
                all_sees.push_back(sees);
            }

            /*
            Assumes that calls to sample_state() for
            derived classes are thread safe!!
            Otherwise truly cursed undefined behaviour
            may happen.
            */
            std::vector<propnet::State> sample()
            {
                std::vector<std::future<propnet::State>> futures (Sampler::SAMPLE_SIZE);
                for (auto& future : futures)
                {
                    future = std::async(
                        std::launch::async,
                        [this]
                        {
                            return to_derived().sample_state();
                        }
                    );
                }

                std::vector<propnet::State> sample (Sampler::SAMPLE_SIZE);
                std::transform(
                    futures.begin(),
                    futures.end(),
                    sample.begin(),
                    [](auto& future)
                    {
                        future.wait();

                        return std::move(future.get());
                    }
                );

                return sample;
            }

            void new_game()
            {
                to_derived().new_game();
            }

            static std::vector<agents::RandomAgent> create_random_agents(const propnet::Propnet& propnet)
            {
                std::vector<agents::RandomAgent> agents {};
                const auto& roles {propnet.get_roles()};
                for (const auto& role : roles)
                {
                    agents.emplace_back(role, propnet);
                }

                return agents;
            }
    };
}
