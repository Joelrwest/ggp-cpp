#include "include/training_sampler.hpp"

namespace rebel
{
    TrainingSampler::TrainingSampler(const propnet::Role& role, const propnet::Propnet& propnet) :
        propnet {propnet},
        role {role}
    {}

    std::vector<propnet::State> TrainingSampler::sample()
    {
        // TODO
        return std::vector<propnet::State> {};
    }

    void TrainingSampler::prepare_new_game()
    {}
};
