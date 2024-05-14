#include "include/role.hpp"

#include <span>
#include <algorithm>

namespace propnet
{
    Role::Role(std::string_view name, const std::vector<std::uint32_t>& sees, const std::unordered_map<std::uint32_t, std::uint32_t>& legal_to_input) :
        name {name},
        sees {sees},
        legals (legal_to_input.size()),
        legal_to_input {legal_to_input}
    {
        std::transform(
            legal_to_input.cbegin(),
            legal_to_input.cend(),
            legals.begin(),
            [](const auto& pair) { return pair.first; }
        );
    }

    std::string_view Role::get_name() const
    {
        return name;
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
};
