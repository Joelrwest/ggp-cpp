#include "include/factory.hpp"
#include "non_seeing.hpp"
#include "human.hpp"
#include "../rebel/include/rebel.hpp"

#include <stdexcept>

namespace agents
{
    std::unique_ptr<Agent> agent_factory(std::string_view name, const propnet::Role& role, const propnet::Propnet& propnet)
    {
        if (name == RandomAgent::NAME)
        {
            return std::make_unique<RandomAgent>(role);
        }
        else if (name == FirstAgent::NAME)
        {
            return std::make_unique<FirstAgent>(role);
        }
        else if (name == LastAgent::NAME)
        {
            return std::make_unique<LastAgent>(role);
        }
        else if (name == HumanAgent::NAME)
        {
            return std::make_unique<HumanAgent>(role);
        }
        else if (name == rebel::RebelAgent<>::NAME)
        {
            return std::make_unique<rebel::RebelAgent<>>(role, propnet);
        }
        else
        {
            throw std::invalid_argument {"Given unkown agent name"};
        }
    }
}
