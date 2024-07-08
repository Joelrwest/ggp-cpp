#include "../include/random_sampler.hpp"
#include "misc.hpp"

#include <algorithm>

namespace rebel
{
RandomSampler::RandomSampler(const propnet::Role &sampler_role, const propnet::Propnet &propnet)
    : propnet{propnet}, sampler_role{sampler_role},
      player_roles{propnet.get_player_roles_excluding(sampler_role.get_id())}, random_role{propnet.get_random_role()}
{
}

void RandomSampler::prepare_new_game()
{
    all_histories.clear();
}

void RandomSampler::add_history(const std::vector<bool> &observation, propnet::PropId prev_input)
{
    all_histories.emplace_back(observation, prev_input);
}

void RandomSampler::pop_history()
{
    all_histories.pop_back();
}

propnet::State RandomSampler::sample_state()
{
    if (all_histories.empty())
    {
        return propnet.create_initial_state();
    }

    const auto state{sample_state_impl(all_histories.begin(), all_histories.end(), propnet.create_initial_state())};

    if (!state.has_value())
    {
        throw std::runtime_error{"Did not find any possible samples"};
    }

    return *state;
}

std::vector<propnet::State> RandomSampler::sample_states(std::size_t num_states)
{
    std::vector<propnet::State> states{};
    std::generate_n(std::back_inserter(states), num_states, [this]() { return sample_state(); });

    return states;
}

RandomSampler::History::History(const std::vector<bool> &observation, propnet::PropId prev_input)
    : observation{observation}, prev_input{prev_input}, invalid_state_cache{}, invalid_state_cache_lock{},
      invalid_inputs_cache{}, invalid_inputs_cache_lock{}
{
}

std::optional<propnet::State> RandomSampler::sample_state_impl(AllHistories::iterator all_histories_it,
                                                               AllHistories::iterator all_histories_end_it,
                                                               propnet::State state)
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
    std::vector<std::vector<propnet::PropId>> randomised_legal_inputs{};
    std::transform(player_roles.begin(), player_roles.end(), std::back_inserter(randomised_legal_inputs),
                   [&state](const auto &player_role) {
                       auto player_inputs{player_role.get_legal_inputs(state)};
                       std::random_shuffle(player_inputs.begin(), player_inputs.end());
                       return player_inputs;
                   });

    if (random_role.has_value())
    {
        auto randoms_inputs{random_role->get_legal_inputs(state)};
        std::random_shuffle(randoms_inputs.begin(), randoms_inputs.end());
        randomised_legal_inputs.push_back(randoms_inputs);
    }

    const auto next_all_histories_it{std::next(all_histories_it, 1)};
    for (misc::LazyCartesianProductGenerator cartesian_product_generator{randomised_legal_inputs};
         cartesian_product_generator.get_is_next(); ++cartesian_product_generator)
    {
        auto inputs{cartesian_product_generator.get()};
        inputs.insert(all_histories_it->prev_input);

        if (is_invalid_inputs(all_histories_it, inputs))
        {
            continue;
        }

        auto next_state{state};
        propnet.take_sees_inputs(next_state, inputs);

        const auto observations{sampler_role.get_observations(next_state)};
        if (observations != all_histories_it->observation)
        {
            add_invalid_inputs(all_histories_it, inputs);
            continue;
        }

        propnet.take_non_sees_inputs(next_state, inputs);

        /*
        Check if this state is invalid
        */
        if (is_invalid_state(all_histories_it, next_state))
        {
            add_invalid_inputs(all_histories_it, inputs);
            continue;
        }

        if (next_all_histories_it == all_histories_end_it)
        {
            return std::optional{next_state};
        }
        else
        {
            const auto sampled_state{sample_state_impl(next_all_histories_it, all_histories_end_it, next_state)};

            if (sampled_state.has_value())
            {
                return sampled_state;
            }
            else
            {
                add_invalid_state(all_histories_it, next_state);
                add_invalid_inputs(all_histories_it, inputs);
            }
        }
    }

    return std::optional<propnet::State>{};
}

bool RandomSampler::is_invalid_state(AllHistories::iterator all_histories_it, const propnet::State &state)
{
    all_histories_it->invalid_state_cache_lock.lock_shared();
    const auto is_invalid_state{all_histories_it->invalid_state_cache.contains(state)};
    all_histories_it->invalid_state_cache_lock.unlock_shared();

    return is_invalid_state;
}

void RandomSampler::add_invalid_state(AllHistories::iterator all_histories_it, const propnet::State &state)
{
    all_histories_it->invalid_state_cache_lock.lock();
    all_histories_it->invalid_state_cache.insert(std::move(state));
    all_histories_it->invalid_state_cache_lock.unlock();
}

bool RandomSampler::is_invalid_inputs(AllHistories::iterator all_histories_it, const propnet::InputSet &inputs)
{
    all_histories_it->invalid_inputs_cache_lock.lock_shared();
    const auto is_invalid_inputs{all_histories_it->invalid_inputs_cache.contains(inputs)};
    all_histories_it->invalid_inputs_cache_lock.unlock_shared();

    return is_invalid_inputs;
}

void RandomSampler::add_invalid_inputs(AllHistories::iterator all_histories_it, const propnet::InputSet &inputs)
{
    all_histories_it->invalid_inputs_cache_lock.lock();
    all_histories_it->invalid_inputs_cache.insert(std::move(inputs));
    all_histories_it->invalid_inputs_cache_lock.unlock();
}
} // namespace rebel
