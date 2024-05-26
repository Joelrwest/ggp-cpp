#pragma once

#include "../sampler.hpp"

#include <optional>

namespace rebel
{
    class NaiveSampler : public Sampler<NaiveSampler>
    {
        public:
            NaiveSampler(const propnet::Role& sampler_role, const propnet::Propnet& propnet);

            void prepare_new_game();
            propnet::State sample_state(const std::vector<std::vector<bool>>& all_observations);
        private:
            std::optional<propnet::State> sample_state_impl(std::vector<std::vector<bool>>::const_iterator all_observations_it, std::vector<std::vector<bool>>::const_iterator all_observations_end_it, propnet::State state);

            const propnet::Propnet& propnet;
            const propnet::Role& sampler_role;
            std::vector<agents::RandomAgent> agents;
    };
}
