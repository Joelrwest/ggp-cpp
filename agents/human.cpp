#include "human.hpp"

#include <iostream>
#include <cstdint>

namespace agents {
    HumanAgent::HumanAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
        Agent {role, propnet},
        role {role},
        propnet {propnet}
    {}

    std::uint32_t HumanAgent::get_legal(const std::vector<bool>& sees, std::span<const std::uint32_t> legals)
    {
        while (true)
        {
            std::cout << "Agent can see:\n";
            auto see_id_it {role.get_sees().begin()};
            auto see_it {sees.begin()};
            while (see_it != sees.end()) // Only checking one of them
            {
                const auto gdl {propnet.get_gdl(*see_id_it)};
                std::cout << '\t' << (*see_it ? "\033[1;32m" : "\033[1;31m") << gdl << "\033[0m\n";

                ++see_id_it;
                ++see_it;
            }

            std::cout << "Legal moves are:\n";
            for (auto legal_it {legals.begin()}; legal_it != legals.end(); ++legal_it)
            {
                const auto option_number {std::distance(legals.begin(), legal_it)};
                std::cout << "\t[" << option_number << "] ";
                std::cout << propnet.get_gdl(*legal_it) << '\n';
            }

            std::uint32_t chosen;
            std::cout << "Select a move by entering an integer: ";
            std::cin >> chosen;
            std::cout << "You've chosen " << chosen << '\n';

            if (chosen < legals.size())
            {
                return legals[chosen];
            }

            std::cout << chosen << " was not found as an option, try again\n";
        }
    }
};
