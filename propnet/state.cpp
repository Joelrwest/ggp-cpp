#include "state.hpp"

namespace propnet
{
    State::State(std::size_t size) :
        data {size}
    {}

    bool State::get_id(std::uint32_t id) const
    {
        return true;
    }
};
