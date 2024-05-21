#pragma once

#include <vector>
#include <string_view>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <span>

namespace propnet
{
    class Role
    {
        public:
            Role(std::string_view name, const std::vector<std::uint32_t>& sees, const std::unordered_map<std::uint32_t, std::uint32_t>& legal_to_input, const std::unordered_map<std::uint32_t, std::uint32_t>& goals_to_value);

            std::string_view get_name() const;
            std::span<const std::uint32_t> get_goals() const;
            std::span<const std::uint32_t> get_sees() const;
            std::span<const std::uint32_t> get_legals() const;
            std::uint32_t get_input(std::uint32_t legal) const;
            std::uint32_t get_goal_value(std::uint32_t goal) const;
        private:
            std::string name;
            std::vector<std::uint32_t> sees;
            std::vector<std::uint32_t> legals;
            std::vector<std::uint32_t> goals;
            std::unordered_map<std::uint32_t, std::uint32_t> legal_to_input;
            std::unordered_map<std::uint32_t, std::uint32_t> goals_to_value;
    };
}
