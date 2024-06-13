#pragma once

#include <optional>

#include "../../propnet/include/propnet.hpp"
#include "../../propnet/include/parser.hpp"
#include "../../agents/include/non_seeing.hpp" // TODO

namespace rebel::opponent_factory
{
    template<typename AgentT, typename... AgentArgsT>
    std::vector<AgentT> create_player_agents(const propnet::Role& sampler_role, const propnet::Propnet& propnet, AgentArgsT... agent_args)
    {
        std::vector<AgentT> agents {};
        const auto& all_roles {propnet.get_roles()};
        for (const auto& role : all_roles)
        {
            const auto is_self_or_random {
                role.get_name() == sampler_role.get_name() ||
                role.get_name() == propnet::Parser::RANDOM_PLAYER_NAME
            };

            if (!is_self_or_random)
            {
                agents.emplace_back(role, agent_args...);
            }
        }

        return agents;
    }

    template<typename AgentT, typename... AgentArgsT>
    std::vector<AgentT> create_player_agents(const propnet::Propnet& propnet, AgentArgsT... agent_args)
    {
        std::vector<AgentT> agents {};
        const auto& all_roles {propnet.get_roles()};
        for (const auto& role : all_roles)
        {
            agents.emplace_back(role, agent_args...);
        }

        return agents;
    }

    // TODO: Why does this have to be inline???
    inline std::optional<agents::RandomAgent> try_create_random_agent(const propnet::Propnet& propnet)
    {
        const auto& all_roles {propnet.get_roles()};
        for (const auto& role : all_roles)
        {
            if (role.get_name() == propnet::Parser::RANDOM_PLAYER_NAME)
            {
                return std::optional<agents::RandomAgent> {role};
            }
        }

        return std::optional<agents::RandomAgent> {};
    }
}
