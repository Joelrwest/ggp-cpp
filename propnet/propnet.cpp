#include "propnet.hpp"
#include "nodes.hpp"

namespace propnet
{
    Propnet::Propnet(
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
        post_transition_nodes {post_transition_nodes},
        initial_state {nodes.size()}
    {
        static const std::unordered_set<std::uint32_t> EMPTY_INPUTS {};
        take_sees_inputs(initial_state, EMPTY_INPUTS);
        take_non_sees_inputs(initial_state, EMPTY_INPUTS);
        initial_state.set_not_is_initial();
    }

    std::uint32_t Propnet::num_nodes() const
    {
        return nodes.size();
    }

    std::span<const Role> Propnet::get_roles() const
    {
        return roles;
    }

    bool Propnet::eval_prop(std::uint32_t id, const State& state) const
    {
        static const std::unordered_set<std::uint32_t> EMPTY_INPUTS {};
        return eval_prop(id, state, EMPTY_INPUTS);
    }

    bool Propnet::eval_prop(std::uint32_t id, const State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        return nodes.at(id)->eval(state, inputs);
    }

    std::string_view Propnet::get_gdl(std::uint32_t proposition) const
    {
        return propositions.at(proposition)->get_gdl();
    }

    void Propnet::take_sees_inputs(State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        for (const auto id : topologically_sorted_nodes)
        {
            if (!post_transition_nodes.contains(id))
            {
                const auto eval {eval_prop(id, state, inputs)};
                state.update(id, eval);
            }
        }
    }

    void Propnet::take_non_sees_inputs(State& state, const std::unordered_set<std::uint32_t>& inputs) const
    {
        for (const auto id : topologically_sorted_nodes)
        {
            const auto eval {eval_prop(id, state, inputs)};
            state.update(id, eval);
        }
    }

    bool Propnet::is_game_over(const State& state) const
    {
        return state.get(terminal);
    }

    State Propnet::create_initial_state() const
    {
        return initial_state;
    }
}
