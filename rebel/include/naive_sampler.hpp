#pragma once

#include "../sampler.hpp"

#include <optional>

namespace rebel
{
    class NaiveSampler : public Sampler<NaiveSampler>
    {
        public:
            NaiveSampler(const propnet::Role& sampler_role, const propnet::Propnet& propnet);
        protected:
            void prepare_new_game();
            void add_history(const std::vector<bool>& observation, std::uint32_t input);
            propnet::State sample_state(const std::vector<bool>& observation) const;
        private:
            struct History
            {
                const std::vector<bool>& observation;
                std::uint32_t input;
            };

            std::optional<propnet::State> sample_state_impl(const std::vector<bool>& observation, std::vector<History>::const_iterator all_histories_it, std::vector<History>::const_iterator all_histories_end_it, propnet::State state);

            std::vector<History> all_histories {};
            const propnet::Propnet& propnet;
            const propnet::Role& sampler_role;
            std::vector<agents::RandomAgent> player_agents;
            std::optional<agents::RandomAgent> random_agent;
    };
}
