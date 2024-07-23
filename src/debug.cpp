#include "../player/include/player.hpp"
#include "../propnet/include/propnet.hpp"
#include "../propnet/include/vector_state.hpp"
#include "setup.hpp"

#include <algorithm>
#include <iostream>

static constexpr auto NUM_GAMES{1};
static constexpr auto PROBLEM_GAME{"meier"};

int main(int argc, char **argv)
{
    const propnet::Propnet propnet{setup::load_propnet(PROBLEM_GAME)};
    const auto agents{setup::create_agents(propnet)};
    for (auto game_count{0}; game_count < NUM_GAMES; ++game_count)
    {
        auto state{propnet.create_initial_state()};
        for (const auto &agent : agents)
        {
            agent->prepare_new_game();
            agent->take_observations(state);
        }

        while (!propnet.is_game_over(state))
        {
            try
            {
                propnet::InputSet inputs{};
                for (const auto &agent : agents)
                {
                    std::cout << inputs << '\n';
                    const auto input{agent->get_legal_input(state)};
                    inputs.add(input);
                }
                std::cout << "Input set created\n\n";

                propnet.take_sees_inputs(state, inputs);
                for (const auto &agent : agents)
                {
                    agent->take_observations(state);
                }
                propnet.take_non_sees_inputs(state, inputs);
            }
            catch (const std::runtime_error &error)
            {
                std::cout << error.what() << '\n';
                std::cout << state << '\n';
                return 1;
            }
        }
    }

    return 0;
}
