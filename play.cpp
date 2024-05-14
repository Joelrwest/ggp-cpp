#include "propnet/include/basenet.hpp"
#include "propnet/include/propnet.hpp"
#include "setup.hpp"

#include <iostream>

int main(void)
{
    const propnet::BaseNet basenet {setup::load_basenet()};
    propnet::Propnet {basenet};

    std::cout << "Choose a role from:\n";
    const auto& roles {basenet.get_roles()};
    for (const auto& role : roles)
    {
        std::cout << role.get_name() << '\n';
        std::cout << role.get_sees().size() << '\n';
    }

    return 0;
}
