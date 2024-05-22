#pragma once

#include "../sampler.hpp"

namespace rebel
{
    class TrainingSampler : public Sampler<TrainingSampler>
    {
        public:
            TrainingSampler(const propnet::Role& role, const propnet::Propnet& propnet);

            std::vector<propnet::State> sample();
            void prepare_new_game();
        private:
            const propnet::Propnet& propnet;
            const propnet::Role& role;
            std::vector<agents::RandomAgent> agents;
    };
};
