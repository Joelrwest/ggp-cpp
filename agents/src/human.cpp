#include "human.hpp"

#include <iostream>

namespace agents {
    HumanAgent::HumanAgent(const propnet::Role& role) :
        Agent {role},
        role {role}
    {}

    propnet::PropId HumanAgent::get_legal_input_impl(std::span<const propnet::PropId> legal_inputs)
    {
        while (true)
        {
            const auto& observations {get_observations_cache()};
            role.print_observations(observations);
            role.print_legal_inputs(legal_inputs);

            std::size_t chosen;
            std::cout << "Select a move by entering an integer: ";
            std::cin >> chosen;
            std::cout << "You've chosen " << chosen << '\n';

            if (chosen < legal_inputs.size())
            {
                return legal_inputs[chosen];
            }

            std::cout << chosen << " was not found as an option, try again\n";
        }
    }
}
