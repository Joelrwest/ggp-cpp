#pragma once

#include "../../agents/include/agent.hpp"
#include "../sampler.hpp"

#include <iostream>

namespace rebel
{
    template<typename StateSamplerT>
    class RebelAgent : public agents::Agent
    {
        private:
            StateSamplerT sampler;
        public:
            RebelAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
                Agent {role, propnet},
                sampler {role, propnet}
            {}

            void prepare_new_game() override
            {
                sampler.prepare_new_game();
            }

            std::uint32_t get_legal_input_impl(std::span<const std::uint32_t> legals)
            {
                const auto& observations {get_observations_cache()};
                (void) legals;
                sampler.add_observation(observations);
                const auto sampled_states {sampler.sample()};
                for (const auto& sampled_state : sampled_states)
                {
                    std::cout << sampled_state << "\n\n";
                    // TODO: Do CFR search
                }

                return 24;
            }
    };
}
