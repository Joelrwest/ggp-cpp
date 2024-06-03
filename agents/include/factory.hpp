#pragma once

#include "agent.hpp"

#include <memory>
#include <string_view>

namespace agents
{
    std::unique_ptr<Agent> agent_factory(std::string_view name, const propnet::Role& role, const propnet::Propnet& propnet);
}
