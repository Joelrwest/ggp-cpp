#include "cfr.hpp"

namespace rebel
{
    InformationSet::InformationSet() :
        policy (),
        regrets {},
        next_information_sets {}
    {}

    InformationSet& InformationSet::get_next_information_sets(std::uint32_t previous_input, const std::vector<bool>& observations)
    {
        return *next_information_sets[previous_input][observations];
    }

    const std::vector<double>& InformationSet::get_policy() const
    {
        return policy;
    }

    VanillaCfr::VanillaCfr(const propnet::Role& cfr_role, const propnet::Propnet& propnet, std::span<const std::uint32_t> legal_inputs, propnet::State root_state) :
        cfr_role {cfr_role},
        propnet {propnet},
        legal_inputs {legal_inputs},
        root_state {root_state},
        player_roles {propnet.get_player_roles()},
        random_role {propnet.get_random_role()},
        state_cache {}
    {}

    std::vector<double> VanillaCfr::search()
    {
        return search(0);
    }

    std::vector<double> VanillaCfr::search(std::uint32_t depth)
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

    MCCfr::MCCfr(const propnet::Propnet& propnet, const propnet::State& root_state) :
        propnet {propnet},
        root_state {root_state},
        base_information_sets (propnet.num_player_roles())
    {}

    std::vector<std::vector<double>> MCCfr::search()
    {
        /*
        Psuedocode from:

        MONTE CARLO SAMPLING AND REGRET MINIMIZATION FOR EQUILIBRIUM
        COMPUTATION AND DECISION-MAKING IN LARGE EXTENSIVE FORM GAMES

        TODO: Remember to add pruning
        */
        std::vector<RoleInformationSet> role_information_sets {};
        const auto& roles {propnet.get_player_roles()};
        auto roles_it {roles.begin()};
        for (auto& base_information_set : base_information_sets)
        {
            role_information_sets.emplace_back(*roles_it, base_information_set);
            ++roles_it;
        }

        for (std::size_t iteration_count {0}; iteration_count < NUM_ITERATIONS; ++iteration_count)
        {
            if (iteration_count > 0)
            {
                // Make sure the information sets are back to the bases
                auto base_information_set_it {base_information_sets.begin()};
                for (auto& role_information_set : role_information_sets)
                {
                    role_information_set.information_set = *base_information_set_it;
                    ++base_information_set_it;
                }
            }

            search_impl(role_information_sets);
        }

        std::vector<std::vector<double>> policies {};
        std::transform(
            role_information_sets.begin(),
            role_information_sets.end(),
            std::back_inserter(policies),
            [](const auto& role_information_set)
            {
                return std::move(role_information_set.information_set.get().get_policy());
            }
        );

        return policies;
    }

    MCCfr::RoleInformationSet::RoleInformationSet(const propnet::Role& role, InformationSet& information_set) :
        role {role},
        information_set {information_set}
    {}

    std::uint32_t MCCfr::search_impl(std::vector<RoleInformationSet>& role_information_sets)
    {
        (void)role_information_sets;
        /*
        1:  Initialize: ∀I ∈ I, ∀a ∈ A(I) : rI [a] ← sI [a] ← 0
        2:  ExternalSampling(h, i):
        3:    if h ∈ Z then return ui(h)
        4:    if P(h) = c then sample a0 and return ExternalSampling(ha0, i)
        5:    Let I be the information set containing h
        6:    σ(I) ← RegretMatching(rI )
        7:    if P(I) = i then
        8:      Let u be an array indexed by actions and uσ ← 0
        9:      for a ∈ A(I) do
        10:       u[a] ← ExternalSampling(ha, i)
        11:       uσ ← uσ + σ(I, a) · u[a]
        12:     for a ∈ A(I) do
        13:       By Equation 4.20, compute r˜(I, a) ← u[a] − uσ
        14:       rI [a] ← rI [a] + ˜r(I, a)
        15:     return uσ
        16:   else
        17:     Sample action a0 from σ(I)
        18:     u ← ExternalSampling(ha0, i)
        19:     for a ∈ A(I) do
        20:       sI [a] ← sI [a] + σ(I, a)
        21:     return u
        */
        return 0;
    }
}
