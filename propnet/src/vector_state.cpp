#include "vector_state.hpp"

namespace propnet
{
State::State() : State{0}
{
}

State::State(std::size_t size) : state(size), is_initial{true}
{
}

State::State(const State &other) : state{other.state}, is_initial{other.is_initial}
{
}

State::State(State &&other) : state{std::move(other.state)}, is_initial{other.is_initial}
{
}

State &State::operator=(const State &other) = default;

State &State::operator=(State &&other) = default;

bool State::operator==(const State &other) const = default;

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

std::vector<std::byte> State::to_bytes() const
{
    std::vector<std::byte> bytes{};
    std::transform(state.begin(), state.end(), std::back_inserter(bytes),
                   [](const auto proposition) { return proposition ? std::byte{1} : std::byte{0}; });

    return bytes;
}

std::ostream &operator<<(std::ostream &os, const State &state)
{
    os << (state.is_initial ? "Initial state\n" : "Not initial state\n");
    for (auto id{0u}; id < state.state.size(); ++id)
    {
        os << id << ": " << state.state.at(id) << ' ';
    }
    return os << '\n';
}
} // namespace propnet

namespace std
{
std::size_t hash<propnet::State>::operator()(const propnet::State &state) const noexcept
{
    return std::hash<std::vector<bool>>{}(state.state);
}
} // namespace std
