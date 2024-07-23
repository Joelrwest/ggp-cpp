#pragma once

#include "types.hpp"

#include <cstdint>
#include <functional>
#include <ostream>
#include <unordered_set>

namespace propnet
{
class InputSet
{
  public:
    InputSet();
    InputSet(PropId input);
    InputSet(const InputSet &inputs) = default;
    InputSet(InputSet &&inputs) = default;
    InputSet(const std::unordered_set<PropId> &inputs);
    InputSet(std::unordered_set<PropId> &&inputs);

    InputSet &operator=(const InputSet &other) = default;
    InputSet &operator=(InputSet &&other) = default;
    bool operator==(const InputSet &other) const = default;

    bool contains(PropId input) const;
    void add(PropId input);

    friend class std::hash<InputSet>;
    friend std::ostream &operator<<(std::ostream &os, const InputSet &inputs);

  private:
    std::unordered_set<PropId> inputs;
};

inline static const InputSet EMPTY_INPUTS{};
} // namespace propnet

namespace std
{
template <> class hash<propnet::InputSet>
{
  public:
    std::size_t operator()(const propnet::InputSet &state) const noexcept;
};
} // namespace std
