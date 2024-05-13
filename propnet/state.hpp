#pragma once

#include "persistent_array.hpp"

// TODO
namespace propnet
{
    class State
    {
        public:
            State(std::size_t size);
            bool get_proposition(std::uint32_t id) const;
            bool get_is_initial_state() const;
        private:
            bool is_initial_state {true};
            PersistentArray<bool> data;
    };
};
