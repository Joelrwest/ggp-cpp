#pragma once

#include "../sampler.hpp"

#include <optional>

namespace rebel
{
    class BogoSampler : public Sampler<BogoSampler>
    {
        public:
            BogoSampler(const propnet::Role& sampler_role, const propnet::Propnet& propnet);

            void prepare_new_game();
            propnet::State sample_state(const std::vector<std::vector<bool>>& all_observations);
        private:
            std::optional<propnet::State> sample_state_impl(const std::vector<std::vector<bool>>& all_observations);

            const propnet::Propnet& propnet;
            propnet::Role sampler_role;
            std::vector<propnet::Role> all_roles;
    };
}
