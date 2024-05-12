#pragma once

#include "../nodes.hpp"

#include <string_view>
#include <vector>
#include <memory>
#include <string>
#include <concepts>

namespace propnet
{
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
        private:
            static constexpr auto GAMES_PATH {"games/json/"};
            static constexpr auto JSON_EXTENSION {"json"};

            static constexpr auto ROLES_KEY {"roles"};
            static constexpr auto ENTRIES_KEY {"entries"};
            static constexpr auto ID_KEY {"id"};
            static constexpr auto TYPE_KEY {"type"};
            static constexpr auto IN_PROPS_KEY {"in"};
            static constexpr auto INS_PROPS_KEY {"ins"};
            static constexpr auto OUT_PROPS_KEY {"out"};
            static constexpr auto OUTS_PROPS_KEY {"outs"};
            static constexpr auto PROP_TYPE_KEY {"proposition_type"};
            static constexpr auto GDL_KEY {"gdl"};
            static constexpr auto VALUE_KEY {"value"};

            template<std::derived_from<Node> T>
            void add_node(T node)
            {
                nodes.push_back(std::make_unique<const T>(node));
            }

            void add_proposition(std::uint32_t id, std::vector<std::uint32_t> ins, std::vector<std::uint32_t> outs, std::string_view type, std::string&& gdl);

            std::vector<std::string> roles;
            std::vector<std::shared_ptr<const Node>> nodes;
    };
};
