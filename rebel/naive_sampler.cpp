#include "include/naive_sampler.hpp"
#include "misc.hpp"

namespace rebel
{
    NaiveSampler::NaiveSampler(const propnet::Role& sampler_role, const propnet::Propnet& propnet) :
        propnet {propnet},
        sampler_role {sampler_role},
        player_agents {create_player_agents<agents::RandomAgent>(sampler_role, propnet)},
        random_agent {try_create_random_agent(propnet)}
    {}

    /*
    No caches or anything to clear
    */
    void NaiveSampler::prepare_new_game() {}

    propnet::State NaiveSampler::sample_state(const std::vector<bool>& observation) const
    {
        if (all_histories.empty())
        {
            return propnet.create_initial_state();
        }

        const auto state {sample_state_impl(
            observation,
            all_histories.cbegin(),
            all_histories.cend(),
            propnet.create_initial_state()
        )};

        if (!state.has_value())
        {
            throw std::runtime_error {"Did not find any possible samples"};
        }

        return *state;
    }

    std::optional<propnet::State> NaiveSampler::sample_state_impl(const std::vector<bool>& observation, std::vector<History>::const_iterator all_histories_it, std::vector<History>::const_iterator all_histories_end_it, propnet::State state)
    {
        std::vector<std::vector<std::uint32_t>> randomised_legal_inputs {};
        std::transform(
            agents.begin(),
            agents.end(),
            std::back_inserter(randomised_legal_inputs),
            [&state](const agents::RandomAgent& agent)
            {
                return agent.get_legal_inputs(state);
            }
        );

        const auto next_all_observations_it {all_observations_it + 1};
        for (misc::CartesianProductGenerator cartesian_product_generator {randomised_legal_inputs}; cartesian_product_generator.is_next(); ++cartesian_product_generator)
        {
            auto next_state {state};
            const auto inputs {cartesian_product_generator.get()};

            propnet.take_sees_inputs(state, inputs);

            const auto observations {sampler_role.get_observations(state)};
            if (observations != *all_observations_it)
            {
                continue;
            }

            propnet.take_non_sees_inputs(state, inputs);

            if (next_all_observations_it == all_observations_end_it)
            {
                return std::optional {next_state};
            }
            else
            {
                const auto sampled_state {sample_state_impl(
                    next_all_observations_it,
                    all_observations_end_it,
                    std::move(next_state)
                )};

                if (sampled_state.has_value())
                {
                    return sampled_state;
                }
            }
        }

        return std::optional<propnet::State> {};
    }
}
