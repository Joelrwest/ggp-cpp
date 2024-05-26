/*
TODO: Clean up as this file is largely copied from agents.cpp,
however the interface is slightly different from that interface.

Would be ideal to have them exist together, but can't
be bothered currently and I don't want to deal with the
virtual functions when we know the concrete types.
*/

#include "sampler_heuristics.hpp"

#include <random>

namespace rebel::sampler_heuristics
{
    static std::mt19937 random_engine {std::random_device {}()};

    std::unordered_set<std::uint32_t> lazy_random(const propnet::Propnet& propnet, const propnet::State& state)
    {
        std::unordered_set<std::uint32_t> inputs {};
        for (const auto& role : propnet.get_roles())
        {
            const auto legal_inputs {role.get_legal_inputs(state)};

            std::uniform_int_distribution<> distribution (0, legal_inputs.size() - 1);
            const auto idx {distribution(random_engine)};

            const auto chosen_input {legal_inputs.at(idx)};
            inputs.insert(chosen_input);
        }

        return inputs;
    }

    std::vector<std::vector<std::uint32_t>> random(const propnet::Propnet& propnet, const propnet::State& state)
    {
        std::vector<std::vector<std::uint32_t>> all_inputs {};
        for (const auto& role : propnet.get_roles())
        {
            const auto legal_inputs {role.get_legal_inputs(state)};

            // std::random_shuffle(legal_inputs.begin(), legal_inputs.end());
            all_inputs.emplace_back(std::move(legal_inputs));
        }

        return all_inputs;
    }
}
