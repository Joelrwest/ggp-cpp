#pragma once

#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

namespace propnet
{
class StateNode
{
  public:
    /*
    Creates a tree of state nodes from [id_lo, id_hi)
    */
    StateNode(std::uint32_t id_lo, std::uint32_t id_hi);
    StateNode(const StateNode &other, std::uint32_t generation, bool value);
    StateNode(const StateNode &other, std::uint32_t generation, std::shared_ptr<StateNode> left,
              std::shared_ptr<StateNode> right);

    bool get(std::uint32_t getting_id) const;
    std::shared_ptr<StateNode> update(std::uint32_t updating_id, std::uint32_t new_generation, bool new_value);
    void into_vector(std::vector<bool> &vector);

  private:
    static constexpr auto DEFAULT_VALUE{false};
    static constexpr auto MAX_ID{std::numeric_limits<std::uint32_t>::max()};

    static std::shared_ptr<StateNode> construct_starting_node(std::uint32_t id_lo, std::uint32_t id_hi);

    std::uint32_t id;
    std::uint32_t generation;
    std::shared_ptr<StateNode> left;
    std::shared_ptr<StateNode> right;
    bool value;
};

class State
{
  public:
    State(std::uint32_t size);
    State(const State &other);

    bool get(std::uint32_t getting_id) const;
    void update(std::uint32_t updating_id, bool new_value);
    std::vector<bool> to_vector() const;
    bool get_is_initial() const;
    void set_not_is_initial();

  private:
    std::uint32_t size;
    std::uint32_t generation;
    std::shared_ptr<StateNode> root;
    bool is_initial;
};
} // namespace propnet
