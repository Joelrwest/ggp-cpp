#include "cfr.hpp"

namespace rebel
{
    InformationSet::InformationSet() :
        policy (),
        regrets {},
        next_information_sets {},
        previous_input {std::nullopt}
    {}

    InformationSet& InformationSet::get_next_information_set(const std::vector<bool>& observations)
    {
        auto& next_information_set_ptr {next_information_sets[get_chosen_input()][observations]};
        if (next_information_set_ptr == nullptr)
        {
            next_information_set_ptr = std::make_unique<InformationSet>();
        }

        return *next_information_set_ptr;
    }

    void InformationSet::choose_input(std::uint32_t input)
    {
        previous_input.emplace(input);
    }

    std::uint32_t InformationSet::get_chosen_input() const
    {
        if (!previous_input.has_value())
        {
            throw std::logic_error {"No previous input to go off of"};
        }

        return *previous_input;
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
        random_role {propnet.get_random_role()}
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
        player_roles {propnet.get_player_roles()},
        random_role {propnet.get_random_role()},
        root_state {root_state},
        base_information_sets (propnet.num_player_roles()) // TODO
    {}

    std::vector<std::vector<double>> MCCfr::search()
    {
        /*
        Psuedocode from:

        MONTE CARLO SAMPLING AND REGRET MINIMIZATION FOR EQUILIBRIUM
        COMPUTATION AND DECISION-MAKING IN LARGE EXTENSIVE FORM GAMES

        TODO: Remember to add pruning
        */
        std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>> current_information_sets {};
        for (auto& [role_id, base_information_set] : base_information_sets)
        {
            current_information_sets.emplace(role_id, base_information_set);
        }

        for (std::size_t iteration_count {0}; iteration_count < NUM_ITERATIONS; ++iteration_count)
        {
            /*
            The iterator to the player whose turn it currently is.
            This being equal to player_roles.end() signifies that it's randoms turn.
            */
            const auto& acting_role_it {player_roles.begin()};

            /*
            The role that's currently traversing.
            */
            for (auto& traversing_role : player_roles)
            {
                search_impl(current_information_sets, traversing_role, acting_role_it, root_state);
            }
        }

        std::vector<std::vector<double>> policies {};
        std::transform(
            base_information_sets.begin(),
            base_information_sets.end(),
            std::back_inserter(policies),
            [](const auto& entry)
            {
                // TODO: This should be average policy, not the end policy
                return std::move(entry.second.get_policy());
            }
        );

        return policies;
    }

    std::int32_t MCCfr::search_impl(
        std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>>& current_information_sets,
        propnet::Role& traversing_role,
        std::vector<propnet::Role>::iterator acting_role_it,
        propnet::State state
    )
    {
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

        // If we just changed the state, check if it's terminal
        const auto is_state_changed {acting_role_it == player_roles.begin()};
        if (is_state_changed && propnet.is_game_over(state))
        {
            // Return the traversers utility here
            return traversing_role.get_reward(state);
        }

        if (acting_role_it == player_roles.end())
        {
            /*
            All players have already given their inputs,
            time for random to act and then to transition to the next state.
            */
            std::unordered_set<std::uint32_t> inputs {};
            if (random_role.has_value())
            {
                const auto randoms_inputs {random_role->get_legal_inputs(state)};
                const auto randoms_input {misc::sample_random(randoms_inputs)};
                inputs.insert(randoms_input);
            }

            for (const auto& [_, current_information_set] : current_information_sets)
            {
                const auto input {current_information_set.get().get_chosen_input()};
                inputs.insert(input);
            }
        }

        // Do regret matching, whatever that means

        if (traversing_role.get_role_id() == acting_role_it->get_role_id())
        {
            //
        }
        else
        {
            //
        }

        return 0;
    }
}
