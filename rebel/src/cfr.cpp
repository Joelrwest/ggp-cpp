#include "cfr.hpp"
#include "../src/opponent_factory.hpp"

namespace rebel
{
    Cfr::Cfr(const propnet::Role& cfr_role, const propnet::Propnet& propnet, std::span<const std::uint32_t> legal_inputs, propnet::State root_state) :
        cfr_role {cfr_role},
        propnet {propnet},
        legal_inputs {legal_inputs},
        root_state {root_state},
        player_agents {opponent_factory::create_player_agents<agents::RandomAgent>(propnet)},
        random_agent {opponent_factory::try_create_random_agent(propnet)},
        state_cache {}
    {}

    std::vector<double> Cfr::search()
    {
        return search(0);
    }

    std::vector<double> Cfr::search(std::uint32_t depth)
    {
        /*
        Using pseudocode from:
        https://arxiv.org/pdf/1303.4441v1
        */
        /*
        r = ~0; probs = ~0; cfv = ~0;
        */
        (void)depth;

        return std::vector<double> (legal_inputs.size(), 0.0);
    }
}
