#pragma once

#include "../nodes.hpp"
#include "role.hpp"

#include <vector>
#include <memory>

namespace propnet
{
    /*
    Class that parses and stores the nodes to
    the propositional network.

    Does not store any state within it.
    The propnet class instead stores a state
    along with a read-only copy of this network.
    */
    class BaseNet
    {
        public:
            /*
            Slightly strange interface, but works well with the parser.
            */
            BaseNet(
                std::vector<Role>&& roles,
                std::vector<std::shared_ptr<const Node>>&& nodes,
                std::shared_ptr<const Node>&& terminal,
                std::vector<std::uint32_t>&& topologically_sorted_nodes
            );

            std::uint32_t num_nodes() const;
            const std::vector<Role>& get_roles() const;
        private:
            std::vector<Role> roles {};
            std::vector<std::shared_ptr<const Node>> nodes {};
            std::shared_ptr<const Node> terminal {nullptr};
            std::vector<std::uint32_t> topologically_sorted_nodes {};
            // std::vector<std::unordered_map<std::uint32_t, std::uint32_t>> legal_to_input {};
    };
};
