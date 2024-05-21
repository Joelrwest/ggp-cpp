#include "vector_state.hpp"

namespace propnet
{
    State::State() :
        State {0}
    {}

    State::State(std::size_t size) :
        state (size),
        is_initial {true}
    {}

    State::State(const State& other) :
        state {other.state},
        is_initial {other.is_initial}
    {}

    bool State::get(std::uint32_t getting_id) const
    {
        return state.at(getting_id);
    }

    void State::update(std::uint32_t updating_id, bool new_value)
    {
        state.at(updating_id) = new_value;
    }

    const std::vector<bool>& State::to_vector() const
    {
        return state;
    }

    bool State::get_is_initial() const
    {
        return is_initial;
    }

    void State::set_not_is_initial()
    {
        is_initial = false;
    }

    std::ostream& operator<<(std::ostream& os, const State& state)
    {
        os << (state.is_initial ? "Initial state\n" : "Not initial state\n");
        for (auto id {0u}; id < state.state.size(); ++id)
        {
            os << id << ": " << state.state.at(id) << '\n';
        }
        return os << '\n';
    }
};
