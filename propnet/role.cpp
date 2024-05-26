#include "include/role.hpp"

#include <span>
#include <algorithm>
#include <numeric>
#include <iostream>

namespace propnet
{
    Role::Role(std::string_view name, const std::vector<See>& sees, const std::vector<Legal>& legals, const std::vector<Goal>& goals) :
        name {name},
        sees {sees},
        legals {legals},
        goals {goals},
        sees_cache (sees.size()),
        inputs_cache (legals.size())
    {}

    std::string_view Role::get_name() const
    {
        return name;
    }

    std::uint32_t Role::get_reward(const State& state) const
    {
        return std::accumulate(
            goals.begin(),
            goals.end(),
            0,
            [this, &state](const auto accumulation, const auto& goal)
            {
                const auto eval {goal.node->eval(state, EMPTY_INPUTS)};
                const auto value {eval ? goal.value : 0};

                return accumulation + value;
            }
        );
    }

    void Role::take_observations(const State& state)
    {
        std::transform(
            sees.begin(),
            sees.end(),
            sees_cache.begin(),
            [this, &state](const auto& see)
            {
                return see.node->eval(state, EMPTY_INPUTS);
            }
        );
    }

    const std::vector<bool>& Role::get_observations() const
    {
        return sees_cache;
    }

    void Role::take_legal_inputs(const State& state)
    {
        auto inputs_cache_end {inputs_cache.begin()};
        for (const auto& legal : legals)
        {
            if (legal.node->eval(state, EMPTY_INPUTS))
            {
                *inputs_cache_end = legal.input;
                ++inputs_cache_end;
            }
        }
        inputs_cache_size = std::distance(inputs_cache.begin(), inputs_cache_end);
    }

    std::span<const std::uint32_t> Role::get_legal_inputs()
    {
        return std::span {inputs_cache.begin(), inputs_cache_size};
    }

    void Role::print_observations() const
    {
        std::cout << "Agent '" << name << "' can see:\n";
        auto sees_it {sees.begin()};
        for (const auto eval : sees_cache)
        {
            const auto gdl {sees_it->node->get_gdl()};
            std::cout << '\t' << (eval ? "\033[1;32m" : "\033[1;31m") << gdl << "\033[0m\n";
            ++sees_it;
        }
    }

    void Role::print_legals() const
    {
        std::cout << "Legal moves for '" << name << "' are:\n";
        auto inputs_cache_it {inputs_cache.begin()};
        for (const auto& legal : legals)
        {
            const auto option_number {std::distance(inputs_cache.begin(), inputs_cache_it)};
            std::cout << "\t[" << option_number << "] ";
            std::cout << legal.node->get_gdl() << '\n';
        }
    }
}
