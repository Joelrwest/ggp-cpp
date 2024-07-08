#pragma once

#include "../src/nodes.hpp"
#include "role.hpp"
#include "vector_state.hpp"

#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include <span>

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
            Propnet(
                const std::vector<Role>& player_roles,
                const std::optional<Role>& random_role,
                const std::vector<std::shared_ptr<const Node>>& nodes,
                PropId terminal,
                const std::vector<PropId>& topologically_sorted_nodes,
                const std::vector<PropId>& non_post_topologically_sorted_nodes
            );

            const std::vector<Role>& get_player_roles() const;
            std::vector<Role> get_player_roles(Role::Id excluding_id) const;
            const std::optional<Role>& get_random_role() const;
            /*
            Take given inputs.

            We assume that all moves are actually legal,
            and that all players make exactly 1 move.
            */
            void take_sees_inputs(State& state, const InputSet& inputs) const;
            void take_non_sees_inputs(State& state, const InputSet& inputs) const;
            bool is_game_over(const State& state) const;
            State create_initial_state() const;
            std::size_t size() const;
            std::size_t num_player_roles() const;
            bool is_randomness() const;
        private:
            const InputSet EMPTY_INPUTS {};

            void take_inputs(State& state, const InputSet& inputs, const std::vector<PropId>& ids) const;

            std::vector<Role> player_roles;
            std::optional<Role> random_role;
            std::vector<std::shared_ptr<const Node>> nodes;
            PropId terminal;
            std::vector<PropId> topologically_sorted_nodes;
            std::vector<PropId> non_post_topologically_sorted_nodes;
            State initial_state;
    };
}
