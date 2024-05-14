#include "include/agent.hpp"

#include <algorithm>

namespace agents {
    Agent::Agent(const propnet::Role& role, const propnet::Propnet& propnet) :
        role {role},
        propnet {propnet},
        sees_cache (role.get_sees().size()),
        legals_cache (role.get_legals().size())
    {}

    std::uint32_t Agent::get_input()
    {
        const std::unordered_set<std::uint32_t> empty_inputs {};

        auto sees {role.get_sees()};
        std::transform(
            sees.begin(),
            sees.end(),
            sees_cache.begin(),
            [this, &empty_inputs](const auto see) { return propnet.eval_prop(see, empty_inputs); }
        );

        const auto legals {role.get_legals()};
        const auto legals_end {std::copy_if(
            legals.begin(),
            legals.end(),
            legals_cache.begin(),
            [this, &empty_inputs](const auto legal) { return propnet.eval_prop(legal, empty_inputs); }
        )};

        const std::span legals_cache_span {legals_cache.begin(), legals_end};
        if (legals_cache_span.empty())
        {
            throw std::runtime_error {"Agent has no moves"};
        }

        const auto legal {get_legal(sees_cache, legals_cache_span)};
        return role.get_input(legal);
    }
};
