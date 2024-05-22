#pragma once

#include "../sampler.hpp"

#include <cache.hpp>
#include <lru_cache_policy.hpp> // TODO: Profile to see if this or LFU should be used
#include <tuple>

namespace rebel
{
    class TrainingSampler : public Sampler<TrainingSampler>
    {
        public:
            TrainingSampler(const propnet::Role& role, const propnet::Propnet& propnet);

            void prepare_new_game();
            propnet::State sample_state();
        private:
            using EmptyT = std::tuple<>;
            using StateCacheT = typename caches::fixed_sized_cache<propnet::State, EmptyT, caches::LRUCachePolicy>;
            // static constexpr auto CACHE_SIZE {2000};

            const propnet::Propnet& propnet;
            const propnet::Role& role;
            std::vector<agents::RandomAgent> agents;
            StateCacheT state_cache;
    };
}
