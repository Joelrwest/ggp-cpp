#pragma once

#include "../propnet/include/vector_state.hpp"
#include "../propnet/include/propnet.hpp"
#include "../agents/include/non_seeing.hpp" // TODO

#include <concepts>
#include <thread>
#include <optional>
#include <mutex>

namespace rebel
{
    template<typename DerivedSamplerT>
    class Sampler;

    template<typename DerivedSamplerT>
    concept DerivedSampler = requires(DerivedSamplerT derived_sampler, const std::vector<std::vector<bool>>& all_sees)
    {
        { derived_sampler.sample_state(all_sees) } -> std::convertible_to<std::vector<propnet::State>>;
    } && requires(DerivedSamplerT derived_sampler)
    {
        { derived_sampler.new_game() } -> std::convertible_to<void>;
    } && std::derived_from<DerivedSamplerT, Sampler<DerivedSamplerT>>;

    template<typename DerivedSamplerT>
    class Sampler
    {
        private:
            static constexpr auto NUM_THREADS {6};
            static constexpr auto SAMPLE_SIZE {36};

            DerivedSamplerT& to_derived()
            {
                return static_cast<DerivedSamplerT&>(*this);
            }

            std::vector<std::vector<bool>> all_observations {};
        public:
            void add_observation(const std::vector<bool>& observation)
            {
                all_observations.push_back(observation);
            }

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
                if (all_observations.empty())
                {
                    /*
                    We expect at least the initial state here,
                    and there's a problem if there isn't.

                    Not an issue here, but could lead to other
                    issues elsewhere if not fixed.
                    */
                    throw std::logic_error {"Sees from initial state not present"};
                }

                std::vector<propnet::State> sample (SAMPLE_SIZE);
                std::vector<std::thread> threads {};
                auto sample_it {sample.begin()};
                std::mutex sample_lock {};
                for (auto thread_count {0}; thread_count < NUM_THREADS; ++thread_count)
                {
                    threads.emplace_back(
                        [this, &sample_it, &sample_lock, sample_end_it = sample.end()]
                        {
                            while (true)
                            {
                                {
                                    const std::lock_guard<std::mutex> sample_guard {sample_lock};
                                    if (sample_it == sample_end_it)
                                    {
                                        return;
                                    }
                                }

                                const propnet::State sampled_state {to_derived().sample_state(all_observations)};

                                const std::lock_guard<std::mutex> sample_guard {sample_lock};
                                *sample_it = std::move(sampled_state);
                                ++sample_it;
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

            void prepare_new_game()
            {
                to_derived().prepare_new_game();
            }
    };
}
