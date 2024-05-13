#pragma once

#include "state_data.hpp"

namespace propnet
{
    class State
    {
        public:
            State(std::uint32_t size);
            State(const State& other);

            bool eval_prop(std::uint32_t id) const;
            bool get_is_initial_state() const;
        private:
            bool is_initial_state;
            StateData data;
    };
};
