#include "include/role.hpp"

namespace propnet
{
    Role::Role(std::string_view name, const std::vector<std::uint32_t>& sees, const std::vector<std::uint32_t>& legals) :
        name {name},
        sees {sees},
        legals {legals}
    {}

    std::string_view Role::get_name() const
    {
        return name;
    }

    const std::vector<std::uint32_t>& Role::get_sees() const
    {
        return sees;
    }

    const std::vector<std::uint32_t>& Role::get_legals() const
    {
        return legals;
    }
};
