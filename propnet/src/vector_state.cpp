#include "vector_state.hpp"

namespace propnet
{
State::State() : State{0}
{
}

State::State(std::size_t size) : state(size), is_initial{true}
{
}

bool State::get(PropId getting_id) const
{
    return state.at(getting_id);
}

void State::update(PropId updating_id, bool new_value)
{
    state.at(updating_id) = new_value;
}

bool State::get_is_initial() const
{
    return is_initial;
}

void State::set_not_is_initial()
{
    is_initial = false;
}

std::ostream &operator<<(std::ostream &os, const State &state)
{
    const auto size{state.state.size()};

    os << (state.is_initial ? "Initial state" : "Not initial state") << ", size is " << size;
    if (size > 0)
    {
        os << ", ";
    }

    for (auto id{0u}; id < size; ++id)
    {
        os << id << ": " << (state.state.at(id) ? 'T' : 'F');
        if (id != size - 1)
        {
            os << ", ";
        }
    }

    return os;
}
} // namespace propnet

namespace std
{
std::size_t hash<propnet::State>::operator()(const propnet::State &state) const noexcept
{
    return std::hash<std::vector<bool>>{}(state.state);
}
} // namespace std
