#pragma once

#include "propnet/include/role.hpp"
#include "propnet/include/propnet.hpp"
#include "agents/include/agent.hpp"

namespace setup
{
    propnet::Propnet load_propnet();
    std::unique_ptr<agents::Agent> create_agent(const propnet::Role& role, const propnet::Propnet& propnet);
    std::vector<std::unique_ptr<agents::Agent>> create_agents(const propnet::Propnet& propnet);
}
