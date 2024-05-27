#pragma once

#include "../sampler.hpp"
#include "../misc.hpp"

#include <tuple>
#include <mutex>

namespace rebel
{
    /*
    Assumes that we don't have any 'loops' in any given games.

    E.g., in chess, if both players move a piece back to the
    same position then the states are assumed to be different.
    */
    class TrainingSampler : public Sampler<TrainingSampler>
    {
        public:
            TrainingSampler(const propnet::Role& role, const propnet::Propnet& propnet);
        protected:
            void prepare_new_game();
            void add_history(const std::vector<bool>& observation, std::uint32_t input);
            propnet::State sample_state();
        private:
            struct History
            {
                const std::vector<bool>& observation;
                std::uint32_t input;
                std::unordered_map<propnet::State, std::vector<std::vector<std::uint32_t>>> move_cache;
                std::mutex move_cache_lock;
            };

            std::vector<History> all_histories {};
            const propnet::Propnet& propnet;
            const propnet::Role& role;
    };
}
