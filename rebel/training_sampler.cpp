#include "include/training_sampler.hpp"

namespace rebel
{
    TrainingSampler::TrainingSampler(const propnet::Role& role, const propnet::Propnet& propnet) :
        propnet {propnet},
        role {role},
        agents {create_random_agents(propnet)},
        state_cache (1)
    {}

    void TrainingSampler::prepare_new_game()
    {}

    propnet::State TrainingSampler::sample_state()
    {
        // TODO
        return propnet::State {};
    }
}
