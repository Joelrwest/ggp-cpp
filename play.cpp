#include "propnet/include/propnet.hpp"
#include "propnet/include/vector_state.hpp"
#include "rebel/include/naive_sampler.hpp"
#include "rebel/include/rebel.hpp"
#include "setup.hpp"

#include <iostream>
#include <algorithm>

static constexpr auto NUM_GAMES {1};

void print_state(const propnet::Propnet& propnet, const propnet::State& state);

int main(void)
{
    const propnet::Propnet propnet {setup::load_propnet()};
    const auto agents {setup::create_agents(propnet)};
    rebel::RebelAgent<rebel::NaiveSampler> rebel_agent {propnet.get_roles().front(), propnet};
    for (auto game_num {0}; game_num < NUM_GAMES; ++game_num)
    {
        auto state {propnet.create_initial_state()};
        while (!propnet.is_game_over(state))
        {
            std::cout << "\nNew round\n";
            std::unordered_set<std::uint32_t> inputs {};
            for (const auto& agent : agents)
            {
                const auto input {agent->get_input(state)};
                std::cout << "An agent took input " << input << '\n';
                inputs.insert(input);
            }

            propnet.take_sees_inputs(state, inputs);
            for (const auto& agent : agents)
            {
                agent->take_observations(state);
            }
            propnet.take_non_sees_inputs(state, inputs);
        }
    }

    return 0;
}

void print_state(const propnet::Propnet& propnet, const propnet::State& state)
{
    for (auto id {0u}; id < propnet.num_nodes(); ++id)
    {
        std::cout << id << ' ' << propnet.eval_prop(id, state, std::unordered_set<std::uint32_t> {}) << '\n';
    }
    std::cout << "\n\n";
}
