#include "simple.hpp"

namespace agents
{
    SimpleAgent::SimpleAgent(const propnet::Role& role) :
        Agent {role},
        role {role}
    {}

    std::vector<std::uint32_t> SimpleAgent::get_legal_inputs(const propnet::State& state) const
    {
        auto legal_inputs {role.get_legal_inputs(state)};
        if (legal_inputs.size() > 1)
        {
            get_legal_inputs_impl(legal_inputs);
        }

        return legal_inputs;
    }
}
