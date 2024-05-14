#pragma once

#include "basenet.hpp"
#include "../state.hpp"

namespace propnet
{
    class Propnet
    {
        public:
            Propnet(const Basenet& basenet);
            Propnet(const Propnet& other) = delete;

            /*
            Take given legals.

            We assume that all moves are actually legal,
            and that all players make exactly 1 move.
            */
            void take_inputs(const std::unordered_set<std::uint32_t>& inputs);
            bool eval_prop(std::uint32_t id, const std::unordered_set<std::uint32_t>& inputs) const;
            std::string_view get_gdl(std::uint32_t proposition) const;
            bool is_game_over() const;
        private:
            const Basenet& basenet;
            State state;
    };
};
