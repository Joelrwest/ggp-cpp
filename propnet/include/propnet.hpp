#pragma once

#include "include/basenet.hpp"
#include "state.hpp"

namespace propnet
{
    class Propnet
    {
        public:
            Propnet(const BaseNet& basenet);
        private:
            const BaseNet& basenet;
            State state;
    };
};
