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
        std::vector<std::uint32_t> legals_cache {};
        for (const auto& role : propnet.get_roles())
        {
            const auto possible_legals {role.get_legals()};
            legals_cache.resize(0);
            std::copy_if(
                possible_legals.begin(),
                possible_legals.end(),
                std::back_inserter(legals_cache),
                [&propnet, &state](const auto legal)
                {
                    return propnet.eval_prop(legal, state);
                }
            );

            if (legals_cache.empty())
            {
                throw std::runtime_error {"No moves to sample"};
            }

            std::uniform_int_distribution<> distribution (0, legals_cache.size() - 1);
            const auto idx {distribution(random_engine)};

            const auto chosen_input {legals_cache.at(idx)};
            inputs.insert(chosen_input);
        }

        return inputs;
    }

    std::vector<std::vector<std::uint32_t>> random(const propnet::Propnet& propnet, const propnet::State& state)
    {
        std::vector<std::vector<std::uint32_t>> all_inputs {};
        for (const auto& role : propnet.get_roles())
        {
            std::vector<std::uint32_t> inputs {};
            for (const auto legal : role.get_legals())
            {
                if (propnet.eval_prop(legal, state))
                {
                    const auto input {role.get_input(legal)};
                    inputs.push_back(input);
                }
            }

            if (inputs.empty())
            {
                throw std::runtime_error {"No moves to sample"};
            }

            std::random_shuffle(inputs.begin(), inputs.end());
            all_inputs.emplace_back(std::move(inputs));
        }

        return all_inputs;
    }
}
