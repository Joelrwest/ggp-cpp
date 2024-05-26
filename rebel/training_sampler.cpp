#include "include/training_sampler.hpp"

#include <algorithm>

namespace rebel
{
    TrainingSampler::TrainingSampler(const propnet::Role& role, const propnet::Propnet& propnet) :
        propnet {propnet},
        role {role},
        invalid_state_cache {}
    {}

    void TrainingSampler::prepare_new_game()
    {
        invalid_state_cache.clear();
    }

    propnet::State TrainingSampler::sample_state(const std::vector<std::vector<bool>>&)
    {
        auto state {propnet.create_initial_state()};
        return state;
        // for (auto& agent : agents)
        // {
        //     agent.prepare_new_game();
        // }

        // for (auto sees_it {std::next(all_sees.begin(), 1)}; sees_it != all_sees.end(); ++sees_it)
        // {
        //     std::unordered_set<std::uint32_t> inputs {};
        //     for (auto& agent : agents)
        //     {
        //         const auto input {agent.get_input(state)};
        //         inputs.insert(input);
        //     }

        //     propnet.take_sees_inputs(state, inputs);

        //     std::vector<bool> curr_sees (sees_it->size());
        //     std::transform(
        //         sees_it->begin(),
        //         sees_it->end(),
        //         curr_sees.begin(),
        //         [this, &state, &inputs](const auto see) { return propnet.eval_prop(see, state, inputs); }
        //     );

        //     propnet.take_non_sees_inputs(state, inputs);

        //     if (invalid_state_cache.Cached(state))
        //     {
        //         return std::optional<propnet::State> {};
        //     }

        //     if (curr_sees != *sees_it)
        //     {
        //         invalid_state_cache.Put(state, Empty {});
        //         return std::optional<propnet::State> {};
        //     }
        // }

        // return std::optional<propnet::State> {state};
    }

    TrainingSampler::InvalidStateCache::InvalidStateCache() :
        UnderlyingInvalidStateCache {CACHE_SIZE}
    {}

    void TrainingSampler::InvalidStateCache::clear()
    {
        Clear();
    }
}
