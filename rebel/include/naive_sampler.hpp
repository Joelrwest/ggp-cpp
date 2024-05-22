#pragma once

#include "../sampler.hpp"

#include <optional>

namespace rebel
{
    class NaiveSampler : public Sampler<NaiveSampler>
    {
        public:
            NaiveSampler(const propnet::Role& role, const propnet::Propnet& propnet);

            void prepare_new_game();
            propnet::State sample_state();
        private:
            const propnet::Propnet& propnet;
            const propnet::Role& role;
            std::vector<agents::RandomAgent> agents;
    };
}
