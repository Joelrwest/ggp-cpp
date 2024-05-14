#include "basenet.hpp"
#include "nodes.hpp"

namespace propnet
{
    Basenet::Basenet(
        std::vector<Role>&& roles,
        std::vector<std::shared_ptr<const Node>>&& nodes,
        std::unordered_map<std::uint32_t, std::shared_ptr<const PropositionNode>>&& propositions,
        std::uint32_t terminal,
        std::vector<std::uint32_t>&& topologically_sorted_nodes,
        std::unordered_set<std::uint32_t> post_transition_nodes
    ) :
        roles {roles},
        nodes {std::move(nodes)},
        propositions {propositions},
        terminal {terminal},
        topologically_sorted_nodes {topologically_sorted_nodes},
        post_transition_nodes {post_transition_nodes}
    {}

    std::uint32_t Basenet::num_nodes() const
    {
        return nodes.size();
    }

    std::span<const Role> Basenet::get_roles() const
    {
        return roles;
    }

    bool Basenet::eval_prop(std::uint32_t id, const State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        return nodes.at(id)->eval(state, inputs);
    }

    std::string_view Basenet::get_gdl(std::uint32_t proposition) const
    {
        return propositions.at(proposition)->get_gdl();
    }

    bool Basenet::is_post_transition_node(std::uint32_t id) const
    {
        return post_transition_nodes.contains(id);
    }

    std::span<const std::uint32_t> Basenet::get_topologically_sorted_nodes() const
    {
        return topologically_sorted_nodes;
    }

    std::uint32_t Basenet::get_terminal() const
    {
        return terminal;
    }
};
