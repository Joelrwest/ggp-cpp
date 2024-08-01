#include "../include/random_sampler.hpp"
#include "misc.hpp"

#include <algorithm>

namespace player
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

void RandomSampler::add_history(const std::vector<bool> &observation, propnet::PropId prev_input,
                                std::span<const propnet::PropId> legal_inputs)
{
    all_histories.emplace_back(observation, prev_input, legal_inputs);
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

    const auto state{sample_state_impl(all_histories.begin(), propnet.create_initial_state())};

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

RandomSampler::History::History(const std::vector<bool> &observation, propnet::PropId prev_input,
                                std::span<const propnet::PropId> legal_inputs)
    : observation{observation}, prev_input{prev_input}, legal_inputs{legal_inputs.begin(), legal_inputs.end()},
      invalid_state_cache{}, invalid_state_cache_lock{}
{
}

RandomSampler::History::History(const RandomSampler::History &other)
    : observation{other.observation}, prev_input{other.prev_input}, legal_inputs{other.legal_inputs},
      invalid_state_cache{}, invalid_state_cache_lock{}
{
    /*
    Awful awful awful and wish this was avoidable...
    just to get around the non-const qualified locks.

    Limited to just this constructor with the locks.
    */
    auto &non_const_other{const_cast<RandomSampler::History &>(other)};

    non_const_other.invalid_state_cache_lock.lock_shared();
    invalid_state_cache_lock.lock();
    invalid_state_cache = other.invalid_state_cache;
    invalid_state_cache_lock.unlock();
    non_const_other.invalid_state_cache_lock.unlock_shared();
}

RandomSampler::History::History(RandomSampler::History &&other)
    : observation{std::move(other.observation)}, prev_input{std::move(other.prev_input)},
      legal_inputs{std::move(other.legal_inputs)}, invalid_state_cache{}, invalid_state_cache_lock{}
{
    other.invalid_state_cache_lock.lock();
    invalid_state_cache_lock.lock();
    invalid_state_cache = std::move(other.invalid_state_cache);
    invalid_state_cache_lock.unlock();

    /*
    Lock is purposely not unlocked.
    Better to have a deadlock than for something undefined to happen
    */
}

RandomSampler::History &RandomSampler::History::operator=(const RandomSampler::History &other)
{
    if (this != &other)
    {
        *this = RandomSampler::History{other};
    }

    return *this;
}

RandomSampler::History &RandomSampler::History::operator=(RandomSampler::History &&other)
{
    if (this != &other)
    {
        observation = std::move(other.observation);
        prev_input = std::move(other.prev_input);

        other.invalid_state_cache_lock.lock();
        invalid_state_cache_lock.lock();
        invalid_state_cache = std::move(other.invalid_state_cache);
        invalid_state_cache_lock.unlock();
    }

    /*
    Ditto with no unlocking other's lock
    */

    return *this;
}

std::optional<propnet::State> RandomSampler::sample_state_impl(AllHistories::iterator all_histories_it,
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

                       static std::mt19937 random_engine{std::random_device{}()};
                       std::shuffle(player_inputs.begin(), player_inputs.end(), random_engine);
                       return player_inputs;
                   });

    if (random_role.has_value())
    {
        auto randoms_inputs{random_role->get_legal_inputs(state)};

        static std::mt19937 random_engine{std::random_device{}()};
        std::shuffle(randoms_inputs.begin(), randoms_inputs.end(), random_engine);
        randomised_legal_inputs.push_back(randoms_inputs);
    }

    const auto next_all_histories_it{std::next(all_histories_it, 1)};
    for (misc::LazyCartesianProductGenerator cartesian_product_generator{randomised_legal_inputs};
         cartesian_product_generator.get_is_next(); ++cartesian_product_generator)
    {
        propnet::InputSet inputs{cartesian_product_generator.get()};
        inputs.add(all_histories_it->prev_input);

        auto next_state{state};
        propnet.take_sees_inputs(next_state, inputs);

        const auto observations{sampler_role.get_observations(next_state)};
        if (observations != all_histories_it->observation)
        {
            continue;
        }

        propnet.take_non_sees_inputs(next_state, inputs);

        /*
        Check if this state is invalid
        */
        if (is_invalid_state(*all_histories_it, next_state) || propnet.is_game_over(state))
        {
            continue;
        }

        const auto legal_inputs{sampler_role.get_legal_inputs(next_state)};
        if (legal_inputs != all_histories_it->legal_inputs)
        {
            continue;
        }

        if (next_all_histories_it == all_histories.end())
        {
            return std::optional{next_state};
        }
        else
        {
            const auto sampled_state{sample_state_impl(next_all_histories_it, next_state)};

            if (sampled_state.has_value())
            {
                return sampled_state;
            }
            else
            {
                add_invalid_state(*all_histories_it, next_state);
            }
        }
    }

    return std::nullopt;
}

bool RandomSampler::is_invalid_state(History &history, const propnet::State &state)
{
    history.invalid_state_cache_lock.lock_shared();
    const auto is_invalid_state{history.invalid_state_cache.contains(state)};
    history.invalid_state_cache_lock.unlock_shared();

    return is_invalid_state;
}

void RandomSampler::add_invalid_state(History &history, const propnet::State &state)
{
    history.invalid_state_cache_lock.lock();
    history.invalid_state_cache.insert(std::move(state));
    history.invalid_state_cache_lock.unlock();
}
} // namespace player
