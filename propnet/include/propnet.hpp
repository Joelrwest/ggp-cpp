#pragma once

#include "../nodes.hpp"
#include "role.hpp"
#include "vector_state.hpp"

#include <vector>
#include <memory>
#include <unordered_map>
#include <span>

namespace propnet
{
    /*
    Class that parses and stores the nodes to
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
                const std::vector<Role>& roles,
                const std::vector<std::shared_ptr<const Node>>& nodes,
                std::uint32_t terminal,
                const std::vector<std::uint32_t>& topologically_sorted_nodes,
                const std::vector<std::uint32_t>& non_post_topologically_sorted_nodes
            );

            const std::vector<Role>& get_roles() const;
            /*
            Take given inputs.

            We assume that all moves are actually legal,
            and that all players make exactly 1 move.
            */
            void take_sees_inputs(State& state, const std::unordered_set<std::uint32_t>& inputs) const;
            void take_non_sees_inputs(State& state, const std::unordered_set<std::uint32_t>& inputs) const;
            bool is_game_over(const State& state) const;
            State create_initial_state() const;
        private:
            const std::unordered_set<std::uint32_t> EMPTY_INPUTS {};

            void take_inputs(State& state, const std::unordered_set<std::uint32_t>& inputs, const std::vector<std::uint32_t>& ids) const;

            std::vector<Role> roles;
            std::vector<std::shared_ptr<const Node>> nodes;
            std::uint32_t terminal;
            std::vector<std::uint32_t> topologically_sorted_nodes;
            std::vector<std::uint32_t> non_post_topologically_sorted_nodes;
            State initial_state;
    };
}
