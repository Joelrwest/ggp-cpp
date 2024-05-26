#pragma once

#include "../propnet/include/propnet.hpp"

#include <unordered_set>
#include <cstdint>

namespace rebel::sampler_heuristics
{
    /*
    Returns a single set of random, but valid, inputs.
    */
    std::unordered_set<std::uint32_t> lazy_random(const propnet::Propnet& propnet, const propnet::State& state);
    /*
    Provides a vector of randomly sorted, but valid, inputs
    for each role.
    */
    std::vector<std::vector<std::uint32_t>> random(const propnet::Propnet& propnet, const propnet::State& state);
}
