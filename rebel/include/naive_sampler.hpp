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
            propnet::State sample_state(const std::vector<std::vector<bool>>& all_sees);
        private:
            std::optional<propnet::State> sample_state_impl(std::vector<std::vector<bool>>::const_iterator all_sees_it, std::vector<std::vector<bool>>::const_iterator all_sees_end_it, propnet::State state);

            const propnet::Propnet& propnet;
            const propnet::Role& role;
    };
}
