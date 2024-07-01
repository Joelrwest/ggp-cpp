#include "../rebel/include/cfr.hpp"
#include "setup.hpp"

#include <iostream>

int main(void)
{
    const propnet::Propnet propnet {setup::load_propnet()};
    std::cout << "Size of propnet = " << propnet.size() << '\n';
    rebel::MCCfr mccfr {propnet};
    const auto initial_state {propnet.create_initial_state()};
    for (auto search_count {0}; search_count < 10; ++search_count)
    {
        int polcy_count {0};
        const auto joint_policy {mccfr.search(initial_state)};
        for (const auto& policy : joint_policy)
        {
            ++polcy_count;
            std::cout << "Policy " << polcy_count << ":\n";
            for (const auto& [input, probability] : policy)
            {
                std::cout << "input = " << input << ", probability = " << probability << '\n';
            }
            std::cout << '\n';
        }
        std::cout << "\n\n";
    }

    return 0;
}
