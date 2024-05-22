#include "simple.hpp"

namespace agents
{
    RandomAgent::RandomAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
        Agent {role, propnet}
    {}

    static std::mt19937 random_engine {std::random_device {}()};
    std::uint32_t RandomAgent::get_legal(const std::vector<bool>&, std::span<const std::uint32_t> legals)
    { // TODO: Go through and convert more stuff to span to be general
        std::uniform_int_distribution<> distribution (0, legals.size() - 1);
        const auto idx {distribution(random_engine)};
        return legals[idx];
    }

    FirstAgent::FirstAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
        Agent {role, propnet}
    {}

    std::uint32_t FirstAgent::get_legal(const std::vector<bool>&, std::span<const std::uint32_t> legals)
    {
        return legals.front();
    }

    LastAgent::LastAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
        Agent {role, propnet}
    {}

    std::uint32_t LastAgent::get_legal(const std::vector<bool>&, std::span<const std::uint32_t> legals)
    {
        return legals.back();
    }
};
