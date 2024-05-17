#include "training_sampler.hpp"

namespace rebel
{
    TrainingSampler::TrainingSampler(const propnet::Propnet& propnet) :
        propnet {propnet}
    {}

    void TrainingSampler::add_sees(const std::vector<bool>& sees)
    {
        all_sees.push_back(sees);
    }
};
