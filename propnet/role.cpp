#include "include/role.hpp"

#include <span>
#include <algorithm>
#include <numeric>
#include <iostream>

namespace propnet
{
    Role::Role(std::string_view name, const std::vector<std::shared_ptr<const PropositionNode>>& sees, const std::vector<Legal>& legals, const std::vector<Goal>& goals) :
        name {name},
        sees {sees},
        legals {legals},
        goals {goals}
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

    std::vector<bool> Role::get_observations(const State& state) const
    {
        std::vector<bool> observations {};
        std::transform(
            sees.begin(),
            sees.end(),
            std::back_inserter(observations),
            [this, &state](const auto& see)
            {
                return see->eval(state, EMPTY_INPUTS);
            }
        );

        return observations;
    }

    void Role::print_observations(const std::vector<bool>& observations) const
    {
        std::cout << "Agent '" << name << "' can see:\n";
        auto sees_it {sees.cbegin()};
        for (const auto observation : observations)
        {
            const auto gdl {(*sees_it)->get_gdl()};
            std::cout << '\t' << (observation ? "\033[1;32m" : "\033[1;31m") << gdl << "\033[0m\n";
            ++sees_it;
        }
    }

    std::vector<std::uint32_t> Role::get_legal_inputs(const State& state) const
    {
        std::vector<std::uint32_t> legal_inputs {};
        for (const auto& legal : legals)
        {
            if (legal.node->eval(state, EMPTY_INPUTS))
            {
                legal_inputs.push_back(legal.input);
            }
        }

        if (legal_inputs.empty())
        {
            throw std::runtime_error {"No legal moves"};
        }

        return legal_inputs;
    }

    void Role::print_legal_inputs(std::span<const std::uint32_t> inputs) const
    {
        std::cout << "Legal moves for '" << name << "' are:\n";
        auto inputs_it {inputs.begin()};
        for (const auto& legal : legals)
        {
            if (*inputs_it == legal.legal)
            {
                const auto option_number {std::distance(inputs.begin(), inputs_it)};
                std::cout << "\t[" << option_number << "] ";
                std::cout << legal.node->get_gdl() << '\n';

                ++inputs_it;
            }
        }
    }
}
