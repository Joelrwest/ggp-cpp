#include "cfr.hpp"
#include "../src/opponent_factory.hpp"

namespace rebel
{
    Cfr::Cfr(const propnet::Role& cfr_role, const propnet::Propnet& propnet) :
        propnet {propnet},
        cfr_role {cfr_role},
        player_agents {opponent_factory::create_player_agents<agents::RandomAgent>(propnet)},
        random_agent {opponent_factory::try_create_random_agent(propnet)},
        state_cache {}
    {}

    std::vector<double> Cfr::search(std::span<const std::uint32_t> legal_inputs, const propnet::State& state)
    {
        /*
        Using pseudocode from:
        https://arxiv.org/pdf/1303.4441v1
        */
        (void)state;
        /*
        r = ~0; probs = ~0; cfv = ~0;
        */
        std::vector<double> policy (legal_inputs.size(), 0.0);
        for (auto iteration_count {0}; iteration_count < NUM_ITERATIONS; ++iteration_count)
        {
            //
        }

        return policy;
    }
}
