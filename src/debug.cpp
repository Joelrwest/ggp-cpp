#include "../rebel/include/search.hpp"
#include "setup.hpp"

#include <iostream>

static constexpr auto BENCHMARK_GAME {"scissor_paper_rock"};

int main(void)
{
    const propnet::Propnet propnet {setup::load_propnet(BENCHMARK_GAME)};
    std::cout << "Size of propnet = " << propnet.size() << '\n';
    rebel::search::ExternalSamplingMCCFR mccfr {propnet};
    const auto initial_state {propnet.create_initial_state()};
    int result_count {0};
    const auto joint_result {mccfr.search(initial_state)};
    for (const auto& result : joint_result)
    {
        ++result_count;
        std::cout << "Policy " << result_count << ":\n";
        for (const auto& [input, probability] : result.first)
        {
            std::cout << "input = " << input << ", probability = " << probability << '\n';
        }
        std::cout << "ev = " << result.second << '\n';
    }
    std::cout << '\n';

    return 0;
}
