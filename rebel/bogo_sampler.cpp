#include "include/bogo_sampler.hpp"
#include "sampler_heuristics.hpp"

namespace rebel
{
    BogoSampler::BogoSampler(const propnet::Propnet& propnet) :
        propnet {propnet}
    {}

    void BogoSampler::prepare_new_game() {}

    propnet::State BogoSampler::sample_state(const std::vector<std::vector<bool>>& all_sees)
    {
        while (true)
        {
            const auto state {sample_state_impl(all_sees)};
            if (state.has_value())
            {
                return *state;
            }
        }
    }

    std::optional<propnet::State> BogoSampler::sample_state_impl(const std::vector<std::vector<bool>>& all_sees)
    {
        auto state {propnet.create_initial_state()};
        for (auto sees_it {std::next(all_sees.begin(), 1)}; sees_it != all_sees.end(); ++sees_it)
        {
            std::unordered_set<std::uint32_t> inputs {sampler_heuristics::lazy_random(propnet, state)};

            propnet.take_sees_inputs(state, inputs);

            std::vector<bool> curr_sees (sees_it->size());
            std::transform(
                sees_it->begin(),
                sees_it->end(),
                curr_sees.begin(),
                [this, &state, &inputs](const auto see) { return propnet.eval_prop(see, state, inputs); }
            );

            if (curr_sees != *sees_it)
            {
                return std::optional<propnet::State> {};
            }

            propnet.take_non_sees_inputs(state, inputs);
        }

        return std::optional<propnet::State> {state};
    }
}
