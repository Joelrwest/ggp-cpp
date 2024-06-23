#include "propnet.hpp"
#include "nodes.hpp"

namespace propnet
{

    Propnet::Propnet(
        const std::vector<Role>& roles,
        const std::vector<std::shared_ptr<const Node>>& nodes,
        std::uint32_t terminal,
        const std::vector<std::uint32_t>& topologically_sorted_nodes,
        const std::vector<std::uint32_t>& non_post_topologically_sorted_nodes
    ) :
        roles {roles},
        nodes {nodes},
        terminal {terminal},
        topologically_sorted_nodes {topologically_sorted_nodes},
        non_post_topologically_sorted_nodes {non_post_topologically_sorted_nodes},
        initial_state {nodes.size()}
    {
        take_sees_inputs(initial_state, EMPTY_INPUTS);
        take_non_sees_inputs(initial_state, EMPTY_INPUTS);
        initial_state.set_not_is_initial();
    }

    const std::vector<Role>& Propnet::get_roles() const
    {
        return roles;
    }

    void Propnet::take_sees_inputs(State& state, const InputSet& inputs) const
    {
        take_inputs(state, inputs, non_post_topologically_sorted_nodes);
    }

    void Propnet::take_non_sees_inputs(State& state, const InputSet& inputs) const
    {
        take_inputs(state, inputs, topologically_sorted_nodes);
    }

    bool Propnet::is_game_over(const State& state) const
    {
        return state.get(terminal);
    }

    State Propnet::create_initial_state() const
    {
        return initial_state;
    }

    std::size_t Propnet::size() const
    {
        return nodes.size();
    }

    std::size_t Propnet::num_roles() const
    {
        return roles.size();
    }

    void Propnet::take_inputs(State& state, const InputSet& inputs, const std::vector<std::uint32_t>& ids) const
    {
        for (const auto id : ids)
        {
            const auto eval {nodes.at(id)->eval(state, inputs)};
            state.update(id, eval);
        }
    }
}
