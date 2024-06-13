#include "../include/bogo_sampler.hpp"
#include "../../propnet/include/input_set.hpp"
#include "opponent_factory.hpp"

namespace rebel
{
    BogoSampler::BogoSampler(const propnet::Role& sampler_role, const propnet::Propnet& propnet) :
        propnet {propnet},
        sampler_role {sampler_role},
        player_agents {opponent_factory::create_player_agents<agents::RandomAgent>(sampler_role, propnet)},
        random_agent {opponent_factory::try_create_random_agent(propnet)}
    {}

    void BogoSampler::prepare_new_game()
    {
        all_histories.clear();
    }

    void BogoSampler::add_history(const std::vector<bool>& observation, std::uint32_t prev_input)
    {
        all_histories.push_back(History {
            .observation = observation,
            .prev_input = prev_input,
        });
    }

    propnet::State BogoSampler::sample_state()
    {
        while (true)
        {
            const auto state {sample_state_impl()};
            if (state.has_value())
            {
                return *state;
            }
        }
    }

    std::optional<propnet::State> BogoSampler::sample_state_impl()
    {
        auto state {propnet.create_initial_state()};
        for (const auto& history : all_histories)
        {
            propnet::InputSet inputs {history.prev_input};
            std::for_each(
                player_agents.begin(),
                player_agents.end(),
                [&inputs, &state](auto& other_agent)
                {
                    const auto input {other_agent.get_legal_input(state)};
                    inputs.add(input);
                }
            );

            if (random_agent.has_value())
            {
                const auto input {random_agent->get_legal_input(state)};
                inputs.add(input);
            }

            propnet.take_sees_inputs(state, inputs);

            const auto observation {sampler_role.get_observations(state)};
            if (observation != history.observation)
            {
                return std::optional<propnet::State> {};
            }

            propnet.take_non_sees_inputs(state, inputs);
        }

        return std::optional<propnet::State> {state};
    }
}
