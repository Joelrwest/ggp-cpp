#pragma once

#include "../sampler.hpp"

// #include "lru_cache/lru_cache.h"

namespace rebel
{
    class TrainingSampler : public Sampler<TrainingSampler>
    {
        public:
            TrainingSampler(const propnet::Role& role, const propnet::Propnet& propnet);

            void prepare_new_game();
            propnet::State sample_state();
        private:
            const propnet::Propnet& propnet;
            const propnet::Role& role;
            std::vector<agents::RandomAgent> agents;
    };
};
