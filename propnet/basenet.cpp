#include "basenet.hpp"
#include "nodes.hpp"

namespace propnet
{
    BaseNet::BaseNet(
        std::vector<Role>&& roles,
        std::vector<std::shared_ptr<const Node>>&& nodes,
        std::uint32_t terminal,
        std::vector<std::uint32_t>&& topologically_sorted_nodes
    ) :
        roles {roles},
        nodes {nodes},
        terminal {terminal},
        topologically_sorted_nodes {topologically_sorted_nodes}
    {}

    std::uint32_t BaseNet::num_nodes() const
    {
        return nodes.size();
    }

    const std::vector<Role>& BaseNet::get_roles() const
    {
        return roles;
    }

    bool BaseNet::eval_prop(std::uint32_t id, const State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        return nodes.at(id)->eval(state, inputs);
    }
};
