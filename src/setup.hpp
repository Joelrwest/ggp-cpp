#pragma once

#include "../propnet/include/role.hpp"
#include "../propnet/include/propnet.hpp"
#include "../agents/include/non_seeing.hpp"

namespace setup
{
    propnet::Propnet load_propnet();
    propnet::Propnet load_propnet(std::string_view game);
    std::unique_ptr<agents::Agent> create_agent(const propnet::Role& role, const propnet::Propnet& propnet);
    std::vector<std::unique_ptr<agents::Agent>> create_agents(const propnet::Propnet& propnet);
    std::unique_ptr<agents::Agent> agent_factory(std::string_view name, const propnet::Role& role, const propnet::Propnet& propnet);
}
