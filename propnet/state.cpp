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

    bool State::get_prop(std::uint32_t id) const
    {
        return data.get(id);
    }

    void State::set_prop(std::uint32_t id, bool new_value)
    {
        data.update(id, new_value);
    }

    bool State::get_is_initial_state() const
    {
        return is_initial_state;
    }

    void State::set_not_initial_state()
    {
        is_initial_state = false;
    }
};
