#include "include/agent.hpp"

#include <algorithm>
#include <iostream>

namespace agents {
    Agent::Agent(const propnet::Role& role, const propnet::Propnet& propnet) :
        role {role},
        propnet {propnet},
        sees_cache (role.get_sees().size()),
        legals_cache (role.get_legals().size())
    {}

    void Agent::prepare_new_game() { /* Most agents do nothing */ }

    void Agent::take_observations(const propnet::State state)
    {
        auto sees {role.get_sees()};
        std::transform(
            sees.begin(),
            sees.end(),
            sees_cache.begin(),
            [this, &state](const auto see) { return propnet.eval_prop(see, state, empty_inputs); }
        );
    }

    std::uint32_t Agent::get_input(const propnet::State state)
    {
        const auto legals {role.get_legals()};
        const auto legals_end {std::copy_if(
            legals.begin(),
            legals.end(),
            legals_cache.begin(),
            [this, &state](const auto legal) { return propnet.eval_prop(legal, state, empty_inputs); }
        )};

        const std::span legals_cache_span {legals_cache.begin(), legals_end};
        if (legals_cache_span.size() == 1)
        {
            const auto legal {legals_cache_span.front()};
            return role.get_input(legal);
        }
        else if (legals_cache_span.empty())
        {
            throw std::runtime_error {"Agent has no moves"};
        }

        const auto legal {get_legal(sees_cache, legals_cache_span)};
        return role.get_input(legal);
    }
};
