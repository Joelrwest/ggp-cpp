#include "include/training_sampler.hpp"

#include <algorithm>

namespace rebel
{
    TrainingSampler::TrainingSampler(const propnet::Role& sampler_role, const propnet::Propnet& propnet) :
        propnet {propnet},
        sampler_role {sampler_role},
        player_agents {create_player_agents<agents::RandomAgent>(sampler_role, propnet)},
        random_agent {try_create_random_agent(propnet)}
    {}

    void TrainingSampler::prepare_new_game()
    {
        all_histories.clear();
    }

    void TrainingSampler::add_history(const std::vector<bool>& observation, std::uint32_t prev_input)
    {
        all_histories.emplace_back(observation, prev_input);
    }

    propnet::State TrainingSampler::sample_state()
    {
        if (all_histories.empty())
        {
            return propnet.create_initial_state();
        }

        const auto state {sample_state_impl(
            all_histories.begin(),
            all_histories.end(),
            propnet.create_initial_state()
        )};

        if (!state.has_value())
        {
            throw std::runtime_error {"Did not find any possible samples"};
        }

        return *state;
    }

    TrainingSampler::History::History(const std::vector<bool>& observation, std::uint32_t prev_input) :
        observation {observation},
        prev_input {prev_input},
        invalid_state_cache {},
        invalid_state_cache_lock {}
    {}

    std::optional<propnet::State> TrainingSampler::sample_state_impl(AllHistories::iterator all_histories_it, AllHistories::iterator all_histories_end_it, propnet::State state)
    {
        /*
        State is assumed to be valid at the beginning.
        It's on the caller to check that.

        If this is violated there shouldn't be any
        correctness issues, but it will be slower.
        */

        /*
        TODO:
        Once the NN is developed, use it here to inform the inputs
        */
        std::vector<std::vector<std::uint32_t>> randomised_legal_inputs {};
        std::transform(
            player_agents.begin(),
            player_agents.end(),
            std::back_inserter(randomised_legal_inputs),
            [&state](const auto& agent)
            {
                return agent.get_legal_inputs(state);
            }
        );

        if (random_agent.has_value())
        {
            randomised_legal_inputs.push_back(random_agent->get_legal_inputs(state));
        }

        const auto next_all_histories_it {std::next(all_histories_it, 1)};
        for (misc::CartesianProductGenerator cartesian_product_generator {randomised_legal_inputs}; cartesian_product_generator.is_next(); ++cartesian_product_generator)
        {
            auto next_state {state};
            auto inputs {cartesian_product_generator.get()};
            inputs.insert(all_histories_it->prev_input);

            propnet.take_sees_inputs(next_state, inputs);

            const auto observations {sampler_role.get_observations(next_state)};
            if (observations != all_histories_it->observation)
            {
                continue;
            }

            propnet.take_non_sees_inputs(next_state, inputs);

            /*
            Check if this state is invalid
            */
            all_histories_it->invalid_state_cache_lock.lock_shared();
            const auto is_invalid {all_histories_it->invalid_state_cache.contains(state)};
            all_histories_it->invalid_state_cache_lock.unlock_shared();
            if (is_invalid)
            {
                continue;
            }

            if (next_all_histories_it == all_histories_end_it)
            {
                return std::optional {next_state};
            }
            else
            {
                const auto sampled_state {sample_state_impl(
                    next_all_histories_it,
                    all_histories_end_it,
                    std::move(next_state)
                )};

                if (sampled_state.has_value())
                {
                    return sampled_state;
                }
            }
        }

        /*
        State is invalid
        */
        all_histories_it->invalid_state_cache_lock.lock();
        all_histories_it->invalid_state_cache.insert(std::move(state));
        all_histories_it->invalid_state_cache_lock.unlock();

        return std::optional<propnet::State> {};
    }
}
