#include "include/role.hpp"

#include <span>
#include <algorithm>

namespace propnet
{
    Role::Role(std::string_view name, const std::vector<std::uint32_t>& sees, const std::unordered_map<std::uint32_t, std::uint32_t>& legal_to_input, const std::unordered_map<std::uint32_t, std::uint32_t>& goals_to_value) :
        name {name},
        sees {sees},
        legals (legal_to_input.size()),
        goals (goals_to_value.size()),
        legal_to_input {legal_to_input},
        goals_to_value {goals_to_value}
    {
        std::transform(
            legal_to_input.cbegin(),
            legal_to_input.cend(),
            legals.begin(),
            [](const auto& pair) { return pair.first; }
        );

        std::transform(
            goals_to_value.cbegin(),
            goals_to_value.cend(),
            goals.begin(),
            [](const auto& pair) { return pair.first; }
        );
    }

    std::string_view Role::get_name() const
    {
        return name;
    }

    std::span<const std::uint32_t> Role::get_goals() const
    {
        return goals;
    }

    std::span<const std::uint32_t> Role::get_sees() const
    {
        return sees;
    }

    std::span<const std::uint32_t> Role::get_legals() const
    {
        return legals;
    }

    std::uint32_t Role::get_input(std::uint32_t legal) const
    {
        return legal_to_input.at(legal);
    }

    std::uint32_t Role::get_goal_value(std::uint32_t goal) const
    {
        return goals_to_value.at(goal);
    }
}
