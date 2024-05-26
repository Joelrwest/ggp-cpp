#include "include/naive_sampler.hpp"

namespace rebel
{
    NaiveSampler::NaiveSampler(const propnet::Role& role, const propnet::Propnet& propnet) :
        propnet {propnet},
        role {role}
    {}

    /*
    No caches or anything to clear
    */
    void NaiveSampler::prepare_new_game() {}

    propnet::State NaiveSampler::sample_state(const std::vector<std::vector<bool>>& all_sees)
    {
        const auto state {sample_state_impl(all_sees.cbegin(), all_sees.cend(), propnet.create_initial_state())};
        if (!state.has_value())
        {
            throw std::runtime_error {"Did not find any possible sample for current sees"};
        }

        return *state;
    }

    std::optional<propnet::State> NaiveSampler::sample_state_impl(std::vector<std::vector<bool>>::const_iterator, std::vector<std::vector<bool>>::const_iterator, propnet::State)
    {
        return std::optional<propnet::State> {};
        // const auto randomised_legal_inputs {sampler_heuristics::random(propnet, state)};

        // auto next_state {state};
        // propnet.take_sees_inputs(state, inputs);

        // const auto observations {sampler_role.get_observations(state)};

        // if (observations != *sees_it)
        // {
        //     return std::optional<propnet::State> {};
        // }

        // propnet.take_non_sees_inputs(state, inputs);

        // auto possible_sampled_state {sample_state_impl(all_sees, std::move(next_state), move_count + 1)};
        // if (possible_sampled_state.has_value())
        // {
        //     return *possible_sampled_state;
        // }
    }
}
