#include "state.hpp"

namespace propnet
{
    State::State(std::size_t size) :
        data {size}
    {}

    bool State::get_proposition(std::uint32_t id) const
    {
        return true;
    }

    bool State::get_is_initial_state() const
    {
        return is_initial_state;
    }
};
