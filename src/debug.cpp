#include "../rebel/include/cfr.hpp"
#include "setup.hpp"

#include <iostream>

int main(void)
{
    const propnet::Propnet propnet {setup::load_propnet()};
    rebel::MCCfr mccfr {propnet};
    const auto initial_state {propnet.create_initial_state()};
    for (auto monkey {0}; monkey < 10; monkey++)
    {
        int count {0};
        const auto joint_policy {mccfr.search(initial_state)};
        for (const auto& policy : joint_policy)
        {
            ++count;
            std::cout << "Policy " << count << ":\n";
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