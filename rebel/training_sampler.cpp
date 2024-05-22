#include "include/training_sampler.hpp"

#include <algorithm>

namespace rebel
{
    TrainingSampler::TrainingSampler(const propnet::Role& role, const propnet::Propnet& propnet) :
        propnet {propnet},
        role {role},
        agents {create_random_agents(propnet)},
        invalid_state_cache {}
    {}

    void TrainingSampler::prepare_new_game()
    {
        invalid_state_cache.clear();
    }

    propnet::State TrainingSampler::sample_state()
    {
        // TODO
        return propnet::State {};
    }

    TrainingSampler::InvalidStateCache::InvalidStateCache() :
        UnderlyingInvalidStateCache {CACHE_SIZE}
    {}

    void TrainingSampler::InvalidStateCache::clear()
    {
        Clear();
    }
}
