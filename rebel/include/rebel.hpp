#pragma once

#include "../../agents/include/agent.hpp"
#include "../sampler.hpp"

#include <iostream>

namespace rebel
{
    template<DerivedSampler StateSamplerT>
    class RebelAgent : public agents::Agent
    {
        private:
            std::optional<std::uint32_t> prev_input;
            StateSamplerT sampler;
        public:
            RebelAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
                Agent {role},
                prev_input {},
                sampler {role, propnet}
            {}

            void prepare_new_game() override
            {
                sampler.prepare_new_game();
            }

            std::uint32_t get_legal_input_impl(std::span<const std::uint32_t> legal_inputs)
            {
                const auto& observation {get_observations_cache()};
                (void) legal_inputs;
                const auto sampled_states {sampler.sample()};
                for (const auto& sampled_state : sampled_states)
                {
                    std::cout << sampled_state << "\n\n";
                    // TODO: Do CFR search
                }
                const auto input {24};
                sampler.add_history(observation, input);

                return input;
            }
    };
}
