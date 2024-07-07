#include "../include/non_seeing.hpp"

namespace agents
{
    NonSeeingAgent::NonSeeingAgent(const propnet::Role& role) :
        SimpleAgent {role}
    {}

    void NonSeeingAgent::take_observations(const propnet::State&) { /* Do nothing instead */ }

    RandomAgent::RandomAgent(const propnet::Role& role) :
        NonSeeingAgent {role}
    {}

    std::uint32_t RandomAgent::get_legal_input_impl(std::span<const std::uint32_t> legal_inputs)
    { // TODO: Go through and convert more stuff to span to be general
        static std::mt19937 random_engine {std::random_device {}()};
        static std::uniform_int_distribution<> distribution (0, legal_inputs.size() - 1);
        const auto idx {distribution(random_engine)};
        return legal_inputs[idx];
    }

    void RandomAgent::get_legal_inputs_impl(std::span<std::uint32_t> legal_inputs) const
    {
        std::random_shuffle(legal_inputs.begin(), legal_inputs.end());
    }

    FirstAgent::FirstAgent(const propnet::Role& role) :
        NonSeeingAgent {role}
    {}

    std::uint32_t FirstAgent::get_legal_input_impl(std::span<const std::uint32_t> legal_inputs)
    {
        return legal_inputs.front();
    }

    void FirstAgent::get_legal_inputs_impl(std::span<std::uint32_t>) const { /* Already in the order we want */ }

    LastAgent::LastAgent(const propnet::Role& role) :
        NonSeeingAgent {role}
    {}

    std::uint32_t LastAgent::get_legal_input_impl(std::span<const std::uint32_t> legal_inputs)
    {
        return legal_inputs.back();
    }

    void LastAgent::get_legal_inputs_impl(std::span<std::uint32_t> legal_inputs) const
    {
        std::reverse(legal_inputs.begin(), legal_inputs.end());
    }
}
