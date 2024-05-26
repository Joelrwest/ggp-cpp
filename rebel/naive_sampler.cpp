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
        const auto state {sample_state_impl(all_sees, propnet.create_initial_state(), 1)};
        if (!state.has_value())
        {
            throw std::runtime_error {"Did not find any possible sample for current sees"};
        }

        return *state;
    }

    std::optional<propnet::State> NaiveSampler::sample_state_impl(const std::vector<std::vector<bool>>& all_sees, propnet::State state, std::uint32_t move_count)
    {
        const auto& sees {all_sees.at(move_count)};
        while (true)
        {
            std::unordered_set<std::uint32_t> inputs {};
            for (auto& agent : agents)
            {
                const auto input {agent.get_input(state)};
                inputs.insert(input);
            }

            auto next_state {state};
            propnet.take_sees_inputs(next_state, inputs);

            std::vector<bool> curr_sees (sees.size());
            std::transform(
                sees.begin(),
                sees.end(),
                curr_sees.begin(),
                [this, &state, &inputs](const auto see) { return propnet.eval_prop(see, state, inputs); }
            );

            if (curr_sees != sees)
            {
                continue;
            }

            propnet.take_non_sees_inputs(next_state, inputs);

            auto possible_sampled_state {sample_state_impl(all_sees, std::move(next_state), move_count + 1)};
            if (possible_sampled_state.has_value())
            {
                return *possible_sampled_state;
            }
        }
    }

    // std::optional<propnet::State> NaiveSampler::sample_state_impl(const std::vector<std::vector<bool>>& all_sees, propnet::State curr_state)
    // {
    //     for (auto sees_it {std::next(all_sees.begin(), 1)}; sees_it != all_sees.end(); ++sees_it)
    //     {
    //         std::unordered_set<std::uint32_t> inputs {};
    //         for (auto& agent : agents)
    //         {
    //             const auto input {agent.get_input(state)};
    //             inputs.insert(input);
    //         }

    //         propnet.take_sees_inputs(state, inputs);

    //         std::vector<bool> curr_sees (sees_it->size());
    //         std::transform(
    //             sees_it->begin(),
    //             sees_it->end(),
    //             curr_sees.begin(),
    //             [this, &state, &inputs](const auto see) { return propnet.eval_prop(see, state, inputs); }
    //         );

    //         if (curr_sees != *sees_it)
    //         {
    //             return std::optional<propnet::State> {};
    //         }

    //         propnet.take_non_sees_inputs(state, inputs);
    //     }

    //     return std::optional<propnet::State> {state};
    // }
}
