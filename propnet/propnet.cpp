#include "include/propnet.hpp"

namespace propnet
{
    Propnet::Propnet(const BaseNet& basenet) :
        basenet {basenet},
        state {basenet.num_nodes()}
    {
        take_inputs(std::unordered_set<std::uint32_t> {});
        state.set_not_initial_state();
    }

    void Propnet::take_inputs(const std::unordered_set<std::uint32_t>& inputs)
    {
        for (const auto id : basenet.get_topologically_sorted_nodes())
        {
            if (!basenet.is_post_transition_node(id))
            {
                const auto eval {basenet.eval_prop(id, state, inputs)};
                state.set_prop(id, eval);
            }
        }

        for (const auto id : basenet.get_topologically_sorted_nodes())
        {
            const auto eval {basenet.eval_prop(id, state, inputs)};
            state.set_prop(id, eval);
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
