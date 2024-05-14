#include "propnet/include/basenet.hpp"
#include "propnet/include/propnet.hpp"
#include "setup.hpp"

#include <iostream>
#include <algorithm>

void print_state(const propnet::BaseNet& basenet, const propnet::Propnet& propnet);

int main(void)
{
    const propnet::BaseNet basenet {setup::load_basenet()};
    propnet::Propnet propnet {basenet};

    const auto agents {setup::create_agents(basenet, propnet)};

    print_state(basenet, propnet);

    return 0;
}

void print_state(const propnet::BaseNet& basenet, const propnet::Propnet& propnet)
{
    for (auto id {0}; id < basenet.num_nodes(); ++id)
    {
        std::cout << id << ' ' << propnet.eval_prop(id, std::unordered_set<std::uint32_t> {}) << '\n';
    }
}
