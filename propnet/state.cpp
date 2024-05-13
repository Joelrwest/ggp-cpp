#include "state.hpp"

namespace propnet
{
    State::State(std::uint32_t size) :
        data {size},
        is_initial_state {true}
    {}

    State::State(const State& other) :
        data {other.data},
        is_initial_state {other.is_initial_state}
    {}

    bool State::eval_prop(std::uint32_t id) const
    {
        return data.get(id);
    }

    bool State::get_is_initial_state() const
    {
        return is_initial_state;
    }
};
