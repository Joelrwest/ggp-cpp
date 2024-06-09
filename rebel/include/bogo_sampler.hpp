#pragma once

#include "../src/sampler.hpp"

#include <optional>
#include <list>

namespace rebel
{
    class BogoSampler : public Sampler
    {
        public:
            BogoSampler(const propnet::Role& sampler_role, const propnet::Propnet& propnet);

            void prepare_new_game();
            void add_history(const std::vector<bool>& observation, std::uint32_t prev_input);
            propnet::State sample_state();
        private:
            struct History
            {
                std::vector<bool> observation;
                std::uint32_t prev_input;
            };

            std::optional<propnet::State> sample_state_impl();

            std::list<History> all_histories {};
            const propnet::Propnet& propnet;
            const propnet::Role sampler_role;
            std::vector<agents::RandomAgent> player_agents;
            std::optional<agents::RandomAgent> random_agent;
    };
}
