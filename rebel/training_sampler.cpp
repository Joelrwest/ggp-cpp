#include "include/training_sampler.hpp"

namespace rebel
{
    TrainingSampler::TrainingSampler(const propnet::Propnet& propnet) :
        Sampler<TrainingSampler> {propnet}
    {}

    const std::vector<propnet::State>& TrainingSampler::sample()
    {}

    void TrainingSampler::clear()
    {}
};
