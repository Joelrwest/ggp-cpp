#include "include/naive_sampler.hpp"

namespace rebel
{
    NaiveSampler::NaiveSampler(const propnet::Role& role, const propnet::Propnet& propnet) :
        propnet {propnet},
        role {role},
        agents {create_random_agents(propnet)}
    {}

    std::vector<propnet::State> NaiveSampler::sample()
    {
        std::vector<propnet::State> sample (Sampler::SAMPLE_SIZE);
        auto sample_it {sample.begin()};
        while (sample_it != sample.end())
        {
            const auto sampled_state {sample_state()};
            if (sampled_state.has_value())
            {
                *sample_it = std::move(sampled_state.value());
                ++sample_it;
            }
        }
    }

    /*
    No caches or anything to clear
    */
    void NaiveSampler::prepare_new_game() {}

    std::vector<agents::RandomAgent> NaiveSampler::create_random_agents(const propnet::Propnet& propnet)
    {
        std::vector<agents::RandomAgent> agents {};
        const auto& roles {propnet.get_roles()};
        for (const auto& role : roles)
        {
            agents.emplace_back(role, propnet);
        }

        return agents;
    }

    std::optional<propnet::State> NaiveSampler::sample_state()
    {
        /*
        Start new games and play until we get
        the same sees as we've got here
        */
        const auto& all_sees {get_all_sees()};
        auto state {propnet.create_initial_state()};

        for (auto& agent : agents)
        {
            agent.prepare_new_game();
        }

        for (const auto& sees : all_sees)
        {
            std::unordered_set<std::uint32_t> inputs {};
            for (auto& agent : agents)
            {
                const auto input {agent.get_input(state)};
                inputs.insert(input);
            }

            propnet.take_sees_inputs(state, inputs);

            std::vector<bool> curr_sees (sees.size());
            std::transform(
                sees.begin(),
                sees.end(),
                curr_sees.begin(),
                [this, &state, &inputs](const auto see) { return propnet.eval_prop(see, state, inputs); }
            );

            if (curr_sees != sees)
            {
                return std::optional<propnet::State> {};
            }

            propnet.take_non_sees_inputs(state, inputs);
        }

        return std::optional {state};
    }
};
