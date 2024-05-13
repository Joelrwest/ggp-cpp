#pragma once

#include "persistent_array.hpp"

// TODO
namespace propnet
{
    class State
    {
        public:
            State(std::size_t size);
            bool get_id(std::uint32_t id) const;
        private:
            PersistentArray<bool> data;
            bool initial_state {true};
    };
};
