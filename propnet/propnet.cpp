#include "include/propnet.hpp"

namespace propnet
{
    Propnet::Propnet(const BaseNet& basenet) :
        basenet {basenet},
        state {basenet.num_nodes()}
    {}
};
