#include "../include/naive_sampler.hpp"
#include "misc.hpp"

namespace rebel
{
    NaiveSampler::NaiveSampler(const propnet::Role& sampler_role, const propnet::Propnet& propnet) :
        propnet {propnet},
        sampler_role {sampler_role},
        player_roles {propnet.get_player_roles(sampler_role.get_role_id())},
        random_role {propnet.get_random_role()}
    {}

    void NaiveSampler::prepare_new_game()
    {
        all_histories.clear();
    }

    void NaiveSampler::add_history(const std::vector<bool>& observation, std::uint32_t prev_input)
    {
        all_histories.push_back(History {
            .observation = observation,
            .prev_input = prev_input,
        });
    }

    propnet::State NaiveSampler::sample_state()
    {
        if (all_histories.empty())
        {
            return propnet.create_initial_state();
        }

        const auto state {sample_state_impl(
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

    std::optional<propnet::State> NaiveSampler::sample_state_impl(AllHistories::const_iterator all_histories_it, AllHistories::const_iterator all_histories_end_it, propnet::State state)
    {
        std::vector<std::vector<std::uint32_t>> randomised_legal_inputs {};
        std::transform(
            player_roles.begin(),
            player_roles.end(),
            std::back_inserter(randomised_legal_inputs),
            [&state](const auto& player_role)
            {
                auto player_inputs {player_role.get_legal_inputs(state)};
                std::random_shuffle(player_inputs.begin(), player_inputs.end());
                return player_inputs;
            }
        );

        if (random_role.has_value())
        {
            auto randoms_inputs {random_role->get_legal_inputs(state)};
            std::random_shuffle(randoms_inputs.begin(), randoms_inputs.end());
            randomised_legal_inputs.push_back(randoms_inputs);
        }

        const auto next_all_histories_it {std::next(all_histories_it, 1)};
        for (misc::LazyCartesianProductGenerator cartesian_product_generator {randomised_legal_inputs}; cartesian_product_generator.get_is_next(); ++cartesian_product_generator)
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

        return std::optional<propnet::State> {};
    }
}
