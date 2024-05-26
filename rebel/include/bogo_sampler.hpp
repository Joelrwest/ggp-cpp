#pragma once

#include "../sampler.hpp"

#include <optional>

namespace rebel
{
    class BogoSampler : public Sampler<BogoSampler>
    {
        public:
            BogoSampler(const propnet::Propnet& propnet);

            void prepare_new_game();
            propnet::State sample_state(const std::vector<std::vector<bool>>& all_sees);
        private:
            std::optional<propnet::State> sample_state_impl(const std::vector<std::vector<bool>>& all_sees);

            const propnet::Propnet& propnet;
    };
}
