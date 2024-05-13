#pragma once

#include "../nodes.hpp"
#include "role.hpp"

#include <nlohmann/json.hpp>
#include <string_view>
#include <vector>
#include <memory>
#include <string>
#include <concepts>
#include <stdexcept>

namespace propnet
{
    class ParsingError : public std::runtime_error
    {
        public:
            ParsingError(const char* message);
    };

    /*
    Class that parses and stores the nodes to
    the propositional network.

    Does not store any state within it.
    The propnet class instead stores a state
    along with a read-only copy of this network.
    */
    class BaseNet
    {
        public:
            BaseNet(std::string_view game);

            std::uint32_t num_nodes() const;
            const std::vector<Role>& get_roles() const;
        private:
            static constexpr auto GAMES_PATH {"games/json/"};
            static constexpr auto JSON_EXTENSION {"json"};

            static constexpr auto ROLES_KEY {"roles"};
            static constexpr auto ROLE_KEY {"role"};
            static constexpr auto SEES_KEY {"sees"};
            static constexpr auto ENTRIES_KEY {"entries"};
            static constexpr auto ID_KEY {"id"};
            static constexpr auto TYPE_KEY {"type"};
            static constexpr auto IN_PROPS_KEY {"in"};
            static constexpr auto INS_PROPS_KEY {"ins"};
            static constexpr auto PROP_TYPE_KEY {"proposition_type"};
            static constexpr auto GDL_KEY {"gdl"};
            static constexpr auto VALUE_KEY {"value"};
            static constexpr auto TOPOLOGICALLY_SORTED_KEY {"topologically_sorted"};

            static constexpr auto INITIAL_PROP_TYPE {"init"};
            static constexpr auto BASE_PROP_TYPE {"base"};
            static constexpr auto INPUT_PROP_TYPE {"input"};
            static constexpr auto LEGAL_PROP_TYPE {"legal"};
            static constexpr auto GOAL_PROP_TYPE {"goal"};
            static constexpr auto SEES_PROP_TYPE {"sees"};
            static constexpr auto TERMINAL_PROP_TYPE {"terminal"};
            static constexpr auto OTHER_PROP_TYPE {"other"};

            template<std::derived_from<Node> T>
            void add_node(T node)
            {
                nodes.push_back(std::make_unique<const T>(node));
            }

            void add_entry(const nlohmann::json& entry);
            void add_proposition(std::uint32_t id, std::string_view type, std::string&& gdl, const nlohmann::json& entry);

            std::vector<Role> roles {};
            std::vector<std::shared_ptr<const Node>> nodes {};
            std::shared_ptr<const Node> terminal {nullptr};
            std::unordered_map<std::string, std::vector<std::uint32_t>> sees {};
            std::vector<std::uint32_t> topologically_sorted_nodes {};
    };
};
