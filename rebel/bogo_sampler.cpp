#include "include/bogo_sampler.hpp"

namespace rebel
{
    BogoSampler::BogoSampler(const propnet::Role& sampler_role, const propnet::Propnet& propnet) :
        propnet {propnet},
        sampler_role {sampler_role}
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
            // TODO
            // std::unordered_set<std::uint32_t> inputs {sampler_heuristics::lazy_random(propnet, state)};
            std::unordered_set<std::uint32_t> inputs {};

            propnet.take_sees_inputs(state, inputs);

            const auto observations {sampler_role.get_observations(state)};

            if (observations != *sees_it)
            {
                return std::optional<propnet::State> {};
            }

            propnet.take_non_sees_inputs(state, inputs);
        }

        return std::optional<propnet::State> {state};
    }
}
