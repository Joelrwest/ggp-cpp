#include "cfr.hpp"

/*
Using pseudocode from:
https://arxiv.org/pdf/1303.4441v1
*/

namespace rebel
{
    Cfr::Cfr(const propnet::Propnet& propnet) :
        propnet {propnet},
        state_cache {}
    {}

    std::vector<double> Cfr::search(std::span<const std::uint32_t> legal_inputs, const propnet::State& state)
    {
        (void)state;
        return std::vector<double> (legal_inputs.size(), 0.0); // TODO
    }
}
