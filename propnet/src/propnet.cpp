#include "propnet.hpp"
#include "nodes.hpp"

namespace propnet
{

    Propnet::Propnet(
        const std::vector<Role>& player_roles,
        const std::optional<Role>& random_role,
        const std::vector<std::shared_ptr<const Node>>& nodes,
        std::uint32_t terminal,
        const std::vector<std::uint32_t>& topologically_sorted_nodes,
        const std::vector<std::uint32_t>& non_post_topologically_sorted_nodes
    ) :
        player_roles {player_roles},
        random_role {random_role},
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

    const std::vector<Role>& Propnet::get_player_roles() const
    {
        return player_roles;
    }

    std::vector<Role> Propnet::get_player_roles(std::uint16_t excluding_id) const
    {
        std::vector<Role> player_roles_excluding {};
        std::copy_if(
            player_roles.begin(),
            player_roles.end(),
            std::back_inserter(player_roles_excluding),
            [excluding_id](const auto& role)
            {
                return role.get_id() == excluding_id;
            }
        );

        return player_roles_excluding;
    }

    const std::optional<Role>& Propnet::get_random_role() const
    {
        return random_role;
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

    std::size_t Propnet::num_player_roles() const
    {
        return player_roles.size();
    }

    bool Propnet::is_randomness() const
    {
        return random_role.has_value();
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
