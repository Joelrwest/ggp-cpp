#include "include/non_seeing.hpp"

namespace agents
{
    NonSeeingAgent::NonSeeingAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
        SimpleAgent {role, propnet}
    {}

    void NonSeeingAgent::take_observations(const propnet::State&) { /* Do nothing instead */ }

    RandomAgent::RandomAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
        NonSeeingAgent {role, propnet}
    {}

    static std::mt19937 random_engine {std::random_device {}()};
    std::uint32_t RandomAgent::get_legal_input_impl(std::span<const std::uint32_t> legals)
    { // TODO: Go through and convert more stuff to span to be general
        std::uniform_int_distribution<> distribution (0, legals.size() - 1);
        const auto idx {distribution(random_engine)};
        return legals[idx];
    }

    void RandomAgent::get_legal_inputs_impl(std::span<std::uint32_t> legal_inputs) const
    {
        std::random_shuffle(legal_inputs.begin(), legal_inputs.end());
    }

    FirstAgent::FirstAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
        NonSeeingAgent {role, propnet}
    {}

    std::uint32_t FirstAgent::get_legal_input_impl(std::span<const std::uint32_t> legals)
    {
        return legals.front();
    }

    void FirstAgent::get_legal_inputs_impl(std::span<std::uint32_t>) const { /* Already in the order we want */ }

    LastAgent::LastAgent(const propnet::Role& role, const propnet::Propnet& propnet) :
        NonSeeingAgent {role, propnet}
    {}

    std::uint32_t LastAgent::get_legal_input_impl(std::span<const std::uint32_t> legals)
    {
        return legals.back();
    }

    void LastAgent::get_legal_inputs_impl(std::span<std::uint32_t> legal_inputs) const
    {
        std::reverse(legal_inputs.begin(), legal_inputs.end());
    }
}
