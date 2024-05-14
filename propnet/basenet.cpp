#include "basenet.hpp"
#include "nodes.hpp"

namespace propnet
{
    BaseNet::BaseNet(
        std::vector<Role>&& roles,
        std::vector<std::shared_ptr<const Node>>&& nodes,
        std::uint32_t terminal,
        std::vector<std::uint32_t>&& topologically_sorted_nodes,
        std::unordered_map<std::uint32_t, std::uint32_t>&& legal_to_input
    ) :
        roles {roles},
        nodes {nodes},
        terminal {terminal},
        topologically_sorted_nodes {topologically_sorted_nodes},
        legal_to_input {legal_to_input}
    {}

    std::uint32_t BaseNet::num_nodes() const
    {
        return nodes.size();
    }

    const std::vector<Role>& BaseNet::get_roles() const
    {
        return roles;
    }
};
