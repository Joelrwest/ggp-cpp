#include "propnet/include/basenet.hpp"
#include "propnet/include/propnet.hpp"
#include "setup.hpp"

#include <iostream>

int main(void)
{
    const propnet::BaseNet basenet {setup::load_basenet()};
    propnet::Propnet propnet {basenet};

    std::cout << "Choose a role from:\n"; // TODO
    const auto& roles {basenet.get_roles()};
    for (const auto& role : roles)
    {
        std::cout << role.get_name() << '\n';
    }

    // Print initial state
    for (auto id {0}; id < basenet.num_nodes(); ++id)
    {
        std::cout << id << ' ' << propnet.eval_prop(id, std::unordered_set<std::uint32_t> {}) << '\n';
    }

    return 0;
}
