#include "../include/agent.hpp"

#include <algorithm>
#include <iostream>

namespace agents {
    Agent::Agent(const propnet::Role& role) :
        role {role}
    {}

    std::uint32_t Agent::get_reward(const propnet::State& state) const
    {
        return role.get_reward(state);
    }

    void Agent::prepare_new_game()
    {
        prev_input.reset();
    }

    void Agent::take_observations(const propnet::State& state)
    {
        // By default agents will see
        observations_cache = role.get_observations(state);
    }

    void Agent::add_history(std::uint32_t) { /* Let the children define if needed */ }

    std::uint32_t Agent::get_legal_input(const propnet::State& state)
    {
        if (prev_input.has_value())
        {
            add_history(*prev_input);
        }

        const auto legal_inputs {role.get_legal_inputs(state)};
        const auto input {
            legal_inputs.size() == 1 ?
            legal_inputs.front() :
            get_legal_input_impl(legal_inputs)
        };
        prev_input.emplace(input);

        return input;
    }

    const std::vector<bool>& Agent::get_observations_cache() const
    {
        return observations_cache;
    }

    std::string_view Agent::get_role_name() const
    {
        return role.get_name();
    }
}
