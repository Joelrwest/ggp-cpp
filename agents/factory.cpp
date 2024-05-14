#include "include/factory.hpp"
#include "simple.hpp"

namespace agents {
    std::unique_ptr<Agent> agent_factory(std::string_view name, const propnet::Role& role, const propnet::Propnet& propnet)
    {
        if (name == "random")
        {
            return std::make_unique<RandomAgent>(role, propnet);
        }

        return nullptr; // TODO
    }
};
