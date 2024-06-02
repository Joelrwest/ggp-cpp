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
            void add_history(const std::vector<bool>& observation, std::uint32_t prev_input);
            propnet::State sample_state();
        private:
            struct History
            {
                const std::vector<bool>& observation;
                std::uint32_t prev_input;
            };

            std::optional<propnet::State> sample_state_impl();

            std::vector<History> all_histories {};
            const propnet::Propnet& propnet;
            const propnet::Role sampler_role;
            std::vector<agents::RandomAgent> player_agents;
            std::optional<agents::RandomAgent> random_agent;
    };
}
