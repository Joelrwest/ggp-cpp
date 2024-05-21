#pragma once

#include "../sampler.hpp"

namespace rebel
{
    class TrainingSampler : public Sampler<TrainingSampler>
    {
        public:
            TrainingSampler(const propnet::Propnet& propnet);

            const std::vector<propnet::State>& sample();
            void clear();
    };
};
