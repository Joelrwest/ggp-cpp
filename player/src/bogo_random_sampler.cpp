#include "../include/bogo_random_sampler.hpp"
#include "../../propnet/include/input_set.hpp"
#include "../include/misc.hpp"

namespace player
{
BogoRandomSampler::BogoRandomSampler(const propnet::Role &sampler_role, const propnet::Propnet &propnet)
    : propnet{propnet}, sampler_role{sampler_role},
      player_roles{propnet.get_player_roles_excluding(sampler_role.get_id())}, random_role{propnet.get_random_role()}
{
}

void BogoRandomSampler::prepare_new_game()
{
    all_histories.clear();
}

void BogoRandomSampler::add_history(const std::vector<bool> &observation, propnet::PropId prev_input)
{
    all_histories.push_back(History{
        .observation = observation,
        .prev_input = prev_input,
    });
}

propnet::State BogoRandomSampler::sample_state()
{
    while (true)
    {
        const auto state{sample_state_impl()};
        if (state.has_value())
        {
            return *state;
        }
    }
}

std::optional<propnet::State> BogoRandomSampler::sample_state_impl()
{
    auto state{propnet.create_initial_state()};
    for (const auto &history : all_histories)
    {
        propnet::InputSet inputs{history.prev_input};
        std::for_each(player_roles.begin(), player_roles.end(), [&inputs, &state](auto &player_role) {
            const auto players_inputs{player_role.get_legal_inputs(state)};
            const auto player_input{misc::sample_random(players_inputs)};
            inputs.add(player_input);
        });

        if (random_role.has_value())
        {
            const auto randoms_inputs{random_role->get_legal_inputs(state)};
            const auto random_input{misc::sample_random(randoms_inputs)};
            inputs.add(random_input);
        }

        propnet.take_sees_inputs(state, inputs);

        const auto observation{sampler_role.get_observations(state)};
        if (observation != history.observation)
        {
            return std::optional<propnet::State>{};
        }

        propnet.take_non_sees_inputs(state, inputs);
    }

    return std::optional<propnet::State>{state};
}
} // namespace player
