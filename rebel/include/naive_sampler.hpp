#pragma once

#include "../sampler.hpp"
#include "../../agents/include/simple.hpp"

namespace rebel
{
    class NaiveSampler : public Sampler<NaiveSampler>
    {
        public:
            NaiveSampler(const propnet::Role& role, const propnet::Propnet& propnet);

            std::vector<propnet::State> sample();
            void prepare_new_game();
        private:
            static std::vector<agents::RandomAgent> create_random_agents(const propnet::Propnet& propnet);
            std::optional<propnet::State> sample_state();

            const propnet::Propnet& propnet;
            const propnet::Role& role;
            std::vector<agents::RandomAgent> agents;
    };
};
