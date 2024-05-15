#include "propnet/include/basenet.hpp"
#include "propnet/include/propnet.hpp"
#include "setup.hpp"

#include <iostream>
#include <algorithm>

void print_state(const propnet::Basenet& basenet, const propnet::Propnet& propnet);

int main(void)
{
    const propnet::Basenet basenet {setup::load_basenet()};
    propnet::Propnet propnet {basenet};

    // print_state(basenet, propnet);

    const auto agents {setup::create_agents(basenet, propnet)};
    while (!propnet.is_game_over())
    {
        std::cout << "\nNew round\n";
        std::unordered_set<std::uint32_t> inputs {};
        for (const auto& agent : agents)
        {
            const auto input {agent->get_input()};
            std::cout << "An agent took input " << input << '\n';
            inputs.insert(input);
        }

        propnet.take_sees_inputs(inputs);
        for (const auto& agent : agents)
        {
            agent->cache_sees();
        }
        propnet.take_non_sees_inputs(inputs);
    }

    return 0;
}

void print_state(const propnet::Basenet& basenet, const propnet::Propnet& propnet)
{
    for (auto id {0}; id < basenet.num_nodes(); ++id)
    {
        std::cout << id << ' ' << propnet.eval_prop(id, std::unordered_set<std::uint32_t> {}) << '\n';
    }
    std::cout << "\n\n";
}
