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
            std::optional<propnet::State> sample_state_impl(const std::vector<std::vector<bool>>& all_sees, propnet::State state, std::uint32_t move_count);

            const propnet::Propnet& propnet;
            const propnet::Role& role;
            std::vector<agents::RandomAgent> agents;
    };
}
