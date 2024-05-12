#include "basenet.hpp"

namespace propnet
{
    /*
    Must be kept in line with the values
    in the Python file games/constants.py
    (which then get outputted to the json variants)
    */
    enum NodeType
    {
        AND = 1,
        OR = 2,
        PROPOSITION = 3,
        TRANSITION = 4,
        NOT = 5,
        CONSTANT = 6
    };

    BaseNet::BaseNet(std::string_view game)
    {
        // TODO
    }
};
