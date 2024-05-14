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
            // bool eval_props(std::vector<std::uint32_t> ids);
            bool eval_prop(std::uint32_t id, const std::unordered_set<std::uint32_t>& inputs) const;

            // def get_actions_for(self, role: str) -> list[Proposition]:
            //     return self.propnet.actions_for[role]

            // def get_legals_for(self, role: str) -> Generator[Proposition, None, None]:
            //     return self.propnet.get_legals_for(role, self.data)

            // def take_actions(self, actions: Generator[Proposition, None, None]) -> None:
            //     actions = (
            //         action.input_id
            //         for action in actions
            //     )
            //     self.propnet.do_step(self.data, actions)
        private:
            const BaseNet& basenet;
            State state;
    };
};
