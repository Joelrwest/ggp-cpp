#pragma once

#include "basenet.hpp"
#include "../state.hpp"

namespace propnet
{
    class Propnet
    {
        public:
            Propnet(const BaseNet& basenet);
            Propnet(const Propnet& other) = delete;

            /*
            Take given legals.

            We assume that all moves are actually legal,
            and that all players make exactly 1 move.
            */
            void take_inputs(const std::unordered_set<std::uint32_t>& inputs);
            bool eval_prop(std::uint32_t id, const std::unordered_set<std::uint32_t>& inputs) const;
            bool is_game_over() const;
        private:
            const BaseNet& basenet;
            State state;
    };
};
