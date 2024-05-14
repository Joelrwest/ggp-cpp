#pragma once

#include "basenet.hpp"

#include <nlohmann/json.hpp>
#include <concepts>
#include <stdexcept>
#include <string_view>
#include <optional>

namespace propnet
{
    class ParsingError : public std::runtime_error
    {
        public:
            ParsingError(const char* message);
    };

    class BaseNetParser
    {
        public:
            BaseNetParser(std::string_view game);

            /*
            Intended to only be called once,
            and will result in a logic error otherwise.
            Essentially the entire parser is invalidated once
            this is called.
            */
            BaseNet create_basenet();
        private:
            static constexpr auto GAMES_PATH {"games/json/"};
            static constexpr auto JSON_EXTENSION {"json"};

            static constexpr auto ROLES_KEY {"roles"};
            static constexpr auto ROLE_KEY {"role"};
            static constexpr auto SEES_KEY {"sees"};
            static constexpr auto LEGAL_TO_INPUT_KEY {"legal_to_input"};
            static constexpr auto LEGAL_KEY {"legal"};
            static constexpr auto INPUT_KEY {"input"};
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
                nodes.emplace_back(std::make_shared<const T>(node));
            }

            void add_entry(const nlohmann::json& entry);
            void add_proposition(std::uint32_t id, std::string_view type, std::string&& gdl, const nlohmann::json& entry);

            bool is_data_valid {true};
            std::vector<Role> roles {};
            std::vector<std::shared_ptr<const Node>> nodes {};
            std::optional<std::uint32_t> terminal {};
            std::vector<std::uint32_t> topologically_sorted_nodes {};
            std::unordered_set<std::uint32_t> post_transition_nodes {};
    };
};
