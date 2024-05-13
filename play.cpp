#include "propnet/include/basenet.hpp"
#include "propnet/include/propnet.hpp"

#include <iostream>

propnet::BaseNet load_basenet();

int main(void)
{
    const propnet::BaseNet basenet {load_basenet()};
    std::cout << "Basenet loaded\n";

    std::cout << "Loading propnet\n";
    propnet::Propnet {basenet};
    std::cout << "Propnet loaded\n";

    std::cout << "Choose a role from:\n";
    const auto& roles {basenet.get_roles()};
    for (const auto& role : roles)
    {
        std::cout << role.get_name() << '\n';
        std::cout << role.get_sees().size() << '\n';
    }

    return 0;
}

propnet::BaseNet load_basenet()
{
    while (true)
    {
        std::string game;
        try
        {
            std::cout << "Enter game name (spelt same as json): ";
            std::cin >> game;

            std::cout << "Loading basenet\n";
            return propnet::BaseNet {game};
        }
        catch (const propnet::ParsingError error)
        {
            std::cout << "Error occurred whilst trying to parse " << game << ": " << error.what() << '\n';
            std::cout << "Please try again.\n";
        }
    }
}
