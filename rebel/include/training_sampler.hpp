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
            /*
            Hacky way to get access to the clear method,
            since it's protected in the implementation...
            */
            using Empty = std::tuple<>;
            using UnderlyingInvalidStateCache = caches::fixed_sized_cache<propnet::State, Empty, caches::LRUCachePolicy>;
            class InvalidStateCache : public UnderlyingInvalidStateCache
            {
                public:
                    InvalidStateCache();

                    void clear();
                private:
                    static constexpr auto CACHE_SIZE {5000}; // TODO: Check if reasonable
            };

            const propnet::Propnet& propnet;
            const propnet::Role& role;
            std::vector<agents::RandomAgent> agents;
            InvalidStateCache invalid_state_cache;
    };
}
