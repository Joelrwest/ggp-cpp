#include "human.hpp"

#include <iostream>
#include <cstdint>

namespace agents {
    HumanAgent::HumanAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
        Agent {role, propnet},
        propnet {propnet}
    {}

    std::uint32_t HumanAgent::get_legal(const std::vector<bool>& sees, std::span<const std::uint32_t> legals)
    {
        while (true)
        {
            for (auto legal_it {legals.begin()}; legal_it != legals.end(); ++legal_it)
            {
                std::cout << "\t[" << std::distance(legals.begin(), legal_it) << "] ";
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
