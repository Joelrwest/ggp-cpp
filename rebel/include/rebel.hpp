#pragma once

#include "../../agents/include/agent.hpp"
#include "../sampler.hpp"

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

            std::uint32_t get_legal(const std::vector<bool>& sees, std::span<const std::uint32_t> legals)
            {
                (void) legals;
                sampler.add_sees(sees);
                const auto sampled_states {sampler.sample()};
                for (const auto& sampled_state : sampled_states)
                {
                    (void) sampled_state;
                    // TODO
                }

                return 0;
            }
    };
}
