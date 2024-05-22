#pragma once

#include "../sampler.hpp"

#include <optional>

namespace rebel
{
    class NaiveSampler : public Sampler<NaiveSampler>
    {
        public:
            NaiveSampler(const propnet::Role& role, const propnet::Propnet& propnet);

            std::vector<propnet::State> sample();
            void prepare_new_game();
        private:
            propnet::State sample_state();

            const propnet::Propnet& propnet;
            const propnet::Role& role;
            std::vector<agents::RandomAgent> agents;
    };
}
