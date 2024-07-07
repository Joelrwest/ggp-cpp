#include "../rebel/include/search.hpp"
#include "setup.hpp"

#include <iostream>

static constexpr auto BENCHMARK_GAME {"blindtictactoe"};

int main(void)
{
    const propnet::Propnet propnet {setup::load_propnet(BENCHMARK_GAME)};
    std::cout << "Size of propnet = " << propnet.size() << '\n';
    rebel::search::ExternalSamplingMCCFR mccfr {propnet};
    const auto initial_state {propnet.create_initial_state()};
    int policy_count {0};
    const auto joint_policy {mccfr.search(initial_state)};
    for (const auto& policy : joint_policy)
    {
        ++policy_count;
        std::cout << "Policy " << policy_count << ":\n";
        for (const auto& [input, probability] : policy)
        {
            std::cout << "input = " << input << ", probability = " << probability << '\n';
        }
        std::cout << '\n';
    }
    std::cout << "\n\n";

    return 0;
}
