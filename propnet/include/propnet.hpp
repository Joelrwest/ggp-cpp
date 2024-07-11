#pragma once

#include "../src/nodes.hpp"
#include "role.hpp"
#include "vector_state.hpp"

#include <memory>
#include <optional>
#include <span>
#include <unordered_map>
#include <vector>

namespace propnet
{
/*
Class that stores the nodes to
the propositional network.

Does not store any state within it.
The propnet class instead stores a state
along with a read-only copy of this network.
*/
class Propnet
{
  public:
    /*
    Slightly strange interface, but works well with the parser.
    */
    Propnet(std::span<const Role> player_roles, const std::optional<Role> &random_role,
            std::span<const std::shared_ptr<const Node>> nodes, PropId terminal,
            std::span<const PropId> topologically_sorted_nodes,
            std::span<const PropId> non_post_topologically_sorted_nodes);
    Propnet(const Propnet &other) = default;
    Propnet(Propnet &&other) = default;

    Propnet &operator=(const Propnet &other) = default;
    Propnet &operator=(Propnet &&other) = default;

    std::span<const Role> get_player_roles() const;
    std::vector<Role> get_player_roles_excluding(Role::Id excluding_id) const;
    const std::optional<Role> &get_random_role() const;
    /*
    Take given inputs.

    We assume that all moves are actually legal,
    and that all players make exactly 1 move.
    */
    void take_sees_inputs(State &state, const InputSet &inputs) const;
    void take_non_sees_inputs(State &state, const InputSet &inputs) const;
    bool is_game_over(const State &state) const;
    State create_initial_state() const;
    std::size_t size() const;
    std::size_t num_player_roles() const;
    bool is_randomness() const;

  private:
    void take_inputs_impl(State &state, const InputSet &inputs, std::span<const PropId> ids) const;

    std::vector<Role> player_roles;
    std::optional<Role> random_role;
    std::vector<std::shared_ptr<const Node>> nodes;
    PropId terminal;
    std::vector<PropId> topologically_sorted_nodes;
    std::vector<PropId> non_post_topologically_sorted_nodes;
    State initial_state;
};
} // namespace propnet
