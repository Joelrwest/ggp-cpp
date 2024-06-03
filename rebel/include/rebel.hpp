#pragma once

#include "../../agents/include/agent.hpp"
#include "../sampler.hpp"
#include "training_sampler.hpp"

#include <iostream>

namespace rebel
{
    template<DerivedSampler StateSamplerT = TrainingSampler>
    class RebelAgent : public agents::Agent
    {
        private:
            std::optional<std::uint32_t> prev_input;
            StateSamplerT sampler;
            const propnet::Role& role;

            // TODO: Only temp for testing, copied from human
            std::uint32_t temp_input_getter(std::span<const std::uint32_t> legal_inputs)
            {
                while (true)
                {
                    const auto& observations {get_observations_cache()};
                    role.print_observations(observations);
                    role.print_legal_inputs(legal_inputs);

                    std::uint32_t chosen;
                    std::cout << "Select a move by entering an integer: ";
                    std::cin >> chosen;
                    std::cout << "You've chosen " << chosen << '\n';

                    if (chosen < legal_inputs.size())
                    {
                        return legal_inputs[chosen];
                    }

                    std::cout << chosen << " was not found as an option, try again\n";
                }
            }
        public:
            RebelAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
                Agent {role},
                prev_input {},
                sampler {role, propnet},
                role {role}
            {}

            static constexpr auto NAME {"rebel"};

            void prepare_new_game() override
            {
                sampler.prepare_new_game();
                prev_input.reset();
            }

            std::uint32_t get_legal_input_impl(std::span<const std::uint32_t> legal_inputs)
            {
                if (prev_input.has_value())
                {
                    const auto& observation {get_observations_cache()};
                    sampler.add_history(observation, *prev_input);
                }

                (void)legal_inputs;
                const auto sampled_states {sampler.sample()};
                for (const auto& sampled_state : sampled_states)
                {
                    // TODO: Do CFR search
                    (void)sampled_state;
                }

                *prev_input = temp_input_getter(legal_inputs); // TODO: Placeholder until rebel decides its own moves
                return *prev_input;
            }
    };
}
