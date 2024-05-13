#include "include/propnet.hpp"

namespace propnet
{
    Propnet::Propnet(const BaseNet& basenet) :
        basenet {basenet},
        state {basenet.num_nodes()}
    {}

    // std::vector<bool> Propnet::eval_props(std::vector<std::uint32_t> ids)
    // {
    //     std::vector<bool> evaluated {};
    //     evaluated.reserve(ids.size());
    //     for (const auto id : ids)
    //     {
    //         const auto eval {state.eval_prop(id)};
    //         evaluated.push_back(eval);
    //     }

    //     return evaluated;
    // }
};
