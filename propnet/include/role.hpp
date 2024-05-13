#pragma once

#include <vector>
#include <string_view>
#include <cstdint>
#include <string>

namespace propnet {
    class Role
    {
        public:
            Role(std::string_view name, const std::vector<std::uint32_t>& sees);

            std::string_view get_name() const;
            const std::vector<std::uint32_t>& get_sees() const;
        private:
            std::string name;
            std::vector<std::uint32_t> sees;
    };
};
