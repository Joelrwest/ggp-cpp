#include "include/factory.hpp"
#include "simple.hpp"
#include "../human.hpp"

#include <stdexcept>

namespace agents {
    std::unique_ptr<Agent> agent_factory(std::string_view name, const propnet::Role& role, const propnet::Propnet& propnet)
    {
        if (name == RandomAgent::NAME)
        {
            return std::make_unique<RandomAgent>(role, propnet);
        }
        else if (name == FirstAgent::NAME)
        {
            return std::make_unique<FirstAgent>(role, propnet);
        }
        else if (name == LastAgent::NAME)
        {
            return std::make_unique<LastAgent>(role, propnet);
        }
        else if (name == HumanAgent::NAME)
        {
            return std::make_unique<HumanAgent>(role, propnet);
        }
        else
        {
            throw std::invalid_argument {"Given unkown agent name"};
        }
    }
}
