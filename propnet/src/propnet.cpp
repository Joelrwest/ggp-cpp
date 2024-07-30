#include "propnet.hpp"
#include "nodes.hpp"

namespace propnet
{
Propnet::Propnet(std::string_view game_name, std::span<const Role> player_roles, const std::optional<Role> &random_role,
                 std::span<const std::shared_ptr<const Node>> nodes, PropId terminal,
                 std::span<const PropId> topologically_sorted_nodes,
                 std::span<const PropId> non_post_topologically_sorted_nodes)
    : game_name{game_name}, player_roles{player_roles.begin(), player_roles.end()},
      random_role{random_role}, nodes{nodes.begin(), nodes.end()}, terminal{terminal},
      topologically_sorted_nodes{topologically_sorted_nodes.begin(), topologically_sorted_nodes.end()},
      non_post_topologically_sorted_nodes{non_post_topologically_sorted_nodes.begin(),
                                          non_post_topologically_sorted_nodes.end()},
      initial_state{nodes.size()}
{
    if (player_roles.empty())
    {
        throw std::runtime_error{"Game loaded has no players"};
    }

    take_sees_inputs(initial_state, EMPTY_INPUTS);
    take_non_sees_inputs(initial_state, EMPTY_INPUTS);
    initial_state.set_not_is_initial();
}

std::string_view Propnet::get_game_name() const
{
    return game_name;
}

std::span<const Role> Propnet::get_player_roles() const
{
    return player_roles;
}

std::vector<Role> Propnet::get_player_roles_excluding(Role::Id excluding_id) const
{
    std::vector<Role> player_roles_excluding{};
    std::copy_if(player_roles.begin(), player_roles.end(), std::back_inserter(player_roles_excluding),
                 [excluding_id](const auto &role) { return role.get_id() != excluding_id; });

    return player_roles_excluding;
}

const std::optional<Role> &Propnet::get_random_role() const
{
    return random_role;
}

bool Propnet::is_game_over(const State &state) const
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

std::size_t Propnet::get_max_policy_size() const
{
    const auto role_it{
        std::max_element(player_roles.begin(), player_roles.end(), [](const auto &role_1, const auto &role_2) {
            return role_1.get_max_policy_size() < role_2.get_max_policy_size();
        })};
    return role_it->get_max_policy_size();
}

void Propnet::take_sees_inputs(State &state, const InputSet &inputs) const
{
    take_inputs_impl(state, inputs, non_post_topologically_sorted_nodes);
}

void Propnet::take_non_sees_inputs(State &state, const InputSet &inputs) const
{
    take_inputs_impl(state, inputs, topologically_sorted_nodes);
}

void Propnet::take_inputs_impl(State &state, const InputSet &inputs, std::span<const PropId> ids) const
{
    for (const auto id : ids)
    {
        const auto eval{nodes.at(id)->eval(state, inputs)};
        state.update(id, eval);
    }
}
} // namespace propnet
