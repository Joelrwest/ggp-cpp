#include "include/naive_sampler.hpp"

namespace rebel
{
    NaiveSampler::NaiveSampler(const propnet::Role& role, const propnet::Propnet& propnet) :
        propnet {propnet},
        role {role},
        agents {create_random_agents(propnet)}
    {}

    /*
    No caches or anything to clear
    */
    void NaiveSampler::prepare_new_game() {}

    propnet::State NaiveSampler::sample_state()
    {
        /*
        Start new games and play until we get
        the same sees as we've got here
        */
        const auto& all_sees {get_all_sees()};
        if (all_sees.empty())
        {
            /*
            We expect at least the initial state here,
            and there's a problem if there isn't.

            Not an issue here, but could lead to other
            issues elsewhere if not fixed.
            */
            throw std::logic_error {"Sees from initial state not present"};
        }

        while (true)
        {
            auto possible_state {true};
            auto state {propnet.create_initial_state()};
            for (auto& agent : agents)
            {
                agent.prepare_new_game();
            }

            for (auto sees_it {std::next(all_sees.begin(), 1)}; sees_it != all_sees.end(); ++sees_it)
            {
                std::unordered_set<std::uint32_t> inputs {};
                for (auto& agent : agents)
                {
                    const auto input {agent.get_input(state)};
                    inputs.insert(input);
                }

                propnet.take_sees_inputs(state, inputs);

                std::vector<bool> curr_sees (sees_it->size());
                std::transform(
                    sees_it->begin(),
                    sees_it->end(),
                    curr_sees.begin(),
                    [this, &state, &inputs](const auto see) { return propnet.eval_prop(see, state, inputs); }
                );

                if (curr_sees != *sees_it)
                {
                    possible_state = false;
                    break;
                }

                propnet.take_non_sees_inputs(state, inputs);
            }

            if (possible_state)
            {
                return state;
            }
        }
    }
};
