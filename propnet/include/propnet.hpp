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
                std::vector<Role>&& roles,
                std::vector<std::shared_ptr<const Node>>&& nodes,
                std::unordered_map<std::uint32_t, std::shared_ptr<const PropositionNode>>&& propositions,
                std::uint32_t terminal,
                std::vector<std::uint32_t>&& topologically_sorted_nodes,
                std::unordered_set<std::uint32_t> post_transition_nodes
            );

            std::uint32_t num_nodes() const;
            std::span<const Role> get_roles() const;
            bool eval_prop(std::uint32_t id, const State& state) const;
            bool eval_prop(std::uint32_t id, const State& state, const std::unordered_set<std::uint32_t>& inputs) const;
            std::string_view get_gdl(std::uint32_t proposition) const;

            /*
            Take given legals.

            We assume that all moves are actually legal,
            and that all players make exactly 1 move.
            */
            void take_sees_inputs(State& state, const std::unordered_set<std::uint32_t>& inputs) const;
            void take_non_sees_inputs(State& state, const std::unordered_set<std::uint32_t>& inputs) const;
            bool is_game_over(const State& state) const;
            State create_initial_state() const;
        private:
            std::vector<Role> roles;
            std::vector<std::shared_ptr<const Node>> nodes; // TODO: Make this unique
            std::unordered_map<std::uint32_t, std::shared_ptr<const PropositionNode>> propositions;
            std::uint32_t terminal;
            std::vector<std::uint32_t> topologically_sorted_nodes;
            std::unordered_set<std::uint32_t> post_transition_nodes;
            State initial_state;
    };
}
