#include "include/propnet.hpp"

namespace propnet
{
    Propnet::Propnet(const BaseNet& basenet) :
        basenet {basenet},
        state {basenet.num_nodes()}
    {}

    void take_inputs(const std::unordered_set<std::uint32_t> legals)
    {
        for (const auto legal : legals)
        {
            //
        }
    }

    bool Propnet::eval_prop(std::uint32_t id, const std::unordered_set<std::uint32_t>& inputs) const
    {
        return basenet.eval_prop(id, state, inputs);
    }

    // std::vector<bool> Propnet::eval_props(std::vector<std::uint32_t> ids)
    // {
    //     std::vector<bool> evald {};
    //     evald.reserve(ids.size());
    //     for (const auto id : ids)
    //     {
    //         const auto eval {state.eval_prop(id)};
    //         evald.push_back(eval);
    //     }

    //     return evald;
    // }
};
