#pragma once

#include "../nodes.hpp"
#include "role.hpp"
#include "../state.hpp"

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
    class Basenet
    {
        public:
            /*
            Slightly strange interface, but works well with the parser.
            */
            Basenet(
                std::vector<Role>&& roles,
                std::vector<std::shared_ptr<const Node>>&& nodes,
                std::unordered_map<std::uint32_t, std::shared_ptr<const PropositionNode>>&& propositions,
                std::uint32_t terminal,
                std::vector<std::uint32_t>&& topologically_sorted_nodes,
                std::unordered_set<std::uint32_t> post_transition_nodes
            );

            std::uint32_t num_nodes() const;
            std::span<const Role> get_roles() const;
            bool eval_prop(std::uint32_t id, const State& state, const std::unordered_set<std::uint32_t>& inputs) const;
            std::string_view get_gdl(std::uint32_t proposition) const;
            bool is_post_transition_node(std::uint32_t id) const;
            std::span<const std::uint32_t> get_topologically_sorted_nodes() const;
            std::uint32_t get_terminal() const;
        private:
            std::vector<Role> roles;
            std::vector<std::shared_ptr<const Node>> nodes; // TODO: Make this unique
            std::unordered_map<std::uint32_t, std::shared_ptr<const PropositionNode>> propositions;
            std::uint32_t terminal;
            std::vector<std::uint32_t> topologically_sorted_nodes;
            std::unordered_set<std::uint32_t> post_transition_nodes;
    };
};
