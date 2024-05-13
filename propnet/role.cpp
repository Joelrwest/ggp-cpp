#include "include/role.hpp"

namespace propnet
{
    Role::Role(std::string_view name, const std::vector<std::uint32_t>& sees) :
        name {name},
        sees {sees}
    {}

    std::string_view Role::get_name() const
    {
        return name;
    }

    const std::vector<std::uint32_t>& Role::get_sees() const
    {
        return sees;
    }
};
