#pragma once

#include "../nodes.hpp"

#include <string_view>
#include <vector>
#include <memory>
#include <string>
#include <concepts>

namespace propnet
{
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
            static constexpr auto VALUE_KEY {"value"};

            template<std::derived_from<Node> T>
            void add_node(T node)
            {
                nodes.push_back(std::make_unique<const T>(node));
            }

            std::vector<std::string> roles;
            std::vector<std::shared_ptr<const Node>> nodes;
    };
};
