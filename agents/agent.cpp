#include "include/agent.hpp"

#include <algorithm>
#include <iostream>

namespace agents {
    Agent::Agent(const propnet::Role& role, const propnet::Propnet& propnet) :
        role {role},
        propnet {propnet}
    {}

    std::uint32_t Agent::get_reward(const propnet::State& state) const
    {
        return role.get_reward(state);
    }

    void Agent::prepare_new_game() { /* Most agents do nothing */ }

    void Agent::take_observations(const propnet::State& state)
    {
        // By default agents will see
        observations_cache = role.get_observations(state);
    }

    std::uint32_t Agent::get_legal_input(const propnet::State& state)
    {
        const auto legal_inputs {role.get_legal_inputs(state)};
        if (legal_inputs.size() == 1)
        {
            return legal_inputs.front();
        }

        const auto input {get_legal_input_impl(legal_inputs)};
        return input;
    }

    const std::vector<bool>& Agent::get_observations_cache() const
    {
        return observations_cache;
    }
}
