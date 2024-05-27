#include "propnet/include/propnet.hpp"
#include "propnet/include/vector_state.hpp"
#include "rebel/include/naive_sampler.hpp"
#include "rebel/include/rebel.hpp"
#include "setup.hpp"

#include <iostream>
#include <algorithm>

static constexpr auto NUM_GAMES {1};

int main(void)
{
    const propnet::Propnet propnet {setup::load_propnet()};
    const auto agents {setup::create_agents(propnet)};
    rebel::RebelAgent<rebel::NaiveSampler> rebel_agent {propnet.get_roles().front(), propnet};
    for (auto game_num {0}; game_num < NUM_GAMES; ++game_num)
    {
        auto state {propnet.create_initial_state()};
        for (const auto& agent : agents)
        {
            agent->prepare_new_game();
            agent->take_observations(state);
        }
        rebel_agent.take_observations(state);
        rebel_agent.prepare_new_game();

        while (!propnet.is_game_over(state))
        {
            std::cout << "Actual state:\n" << state << '\n';
            std::unordered_set<std::uint32_t> inputs {};
            for (const auto& agent : agents)
            {
                const auto input {agent->get_legal_input(state)};
                inputs.insert(input);
            }
            rebel_agent.get_legal_input(state);

            propnet.take_sees_inputs(state, inputs);
            for (const auto& agent : agents)
            {
                agent->take_observations(state);
            }
            rebel_agent.take_observations(state);
            propnet.take_non_sees_inputs(state, inputs);
        }

        for (const auto& agent : agents)
        {
            std::cout << "Reward for '" << agent->get_role_name() << "': " << agent->get_reward(state) << '\n';
        }
    }

    return 0;
}
