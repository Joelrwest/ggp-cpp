#pragma once

#include "types.hpp"

#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <vector>

namespace propnet
{
class State
{
  public:
    State();
    State(std::size_t size);
    State(const State &other) = default;
    State(State &&other) = default;

    State &operator=(const State &other) = default;
    State &operator=(State &&other) = default;
    bool operator==(const State &other) const = default;

    bool get(PropId getting_id) const;
    void update(PropId updating_id, bool new_value);
    bool get_is_initial() const;
    void set_not_is_initial();

    template <typename T> std::vector<T> to_vec() const;

    friend std::ostream &operator<<(std::ostream &os, const State &state);
    friend class std::hash<State>;

  private:
    std::vector<bool> state;
    bool is_initial;
};

template <typename T> std::vector<T> State::to_vec() const
{
    std::vector<T> vec{};
    std::transform(state.begin(), state.end(), std::back_inserter(vec),
                   [](const auto proposition) { return proposition ? T{1} : T{0}; });

    return vec;
}
} // namespace propnet

namespace std
{
template <> class hash<propnet::State>
{
  public:
    std::size_t operator()(const propnet::State &state) const noexcept;
};
} // namespace std
