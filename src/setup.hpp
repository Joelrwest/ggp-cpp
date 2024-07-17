#pragma once

#include "../agents/include/non_seeing.hpp"
#include "../player/include/model.hpp"
#include "../propnet/include/propnet.hpp"
#include "../propnet/include/role.hpp"

namespace setup
{
propnet::Propnet load_propnet();
propnet::Propnet load_propnet(std::string_view game);
std::unique_ptr<agents::Agent> create_agent(const propnet::Propnet &propnet);
std::unique_ptr<agents::Agent> create_agent(const propnet::Role &role);
std::vector<std::unique_ptr<agents::Agent>> create_agents(const propnet::Propnet &propnet);
std::unique_ptr<agents::Agent> agent_factory(std::string_view name, const propnet::Role &role);
player::Model load_model(const propnet::Propnet &propnet, std::string_view game);
} // namespace setup
