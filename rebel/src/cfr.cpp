#include "cfr.hpp"

namespace rebel
{
    InformationSet::InformationSet(const std::vector<std::uint32_t>& legal_inputs) :
        is_choice {legal_inputs.size() > 1},
        cumulative_policy {is_choice ? make_zeroed_map(legal_inputs) : std::unordered_map<uint32_t, double> {}},
        regrets {is_choice ? make_zeroed_map(legal_inputs) : std::unordered_map<uint32_t, double> {}},
        next_information_sets {},
        previous_input {std::nullopt}
    {}

    InformationSet& InformationSet::get_next_information_set(const std::vector<bool>& observations, std::vector<std::uint32_t>&& legal_inputs)
    {
        auto& next_information_set_ptr {next_information_sets[get_chosen_input()][observations]};
        if (next_information_set_ptr == nullptr)
        {
            next_information_set_ptr = std::make_unique<InformationSet>(std::move(legal_inputs));
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

    std::unordered_map<std::uint32_t, double> InformationSet::regret_match() const
    {
        const auto total_positive_regret {std::accumulate(
            regrets.begin(),
            regrets.end(),
            0.0,
            [](const auto& regret_pair)
            {
                return std::max(regret_pair.second, 0.0);
            }
        )};

        std::unordered_map<std::uint32_t, double> policy {};
        if (total_positive_regret > 0.0)
        {
            const auto num_inputs {regrets.size()};
            const auto uniform_probability {1.0 / num_inputs};
            for (const auto& [input, _] : regrets)
            {
                policy.emplace(input, uniform_probability);
            }
        }
        else
        {
            for (const auto& [input, regret] : regrets)
            {
                const auto probability {regret > 0.0 ? regret / total_positive_regret : 0.0};
                policy.emplace(input, probability);
            }
        }

        return policy;
    }

    std::unordered_map<std::uint32_t, double> InformationSet::make_zeroed_map(const std::vector<std::uint32_t>& legal_inputs)
    {
        std::unordered_map<std::uint32_t, double> regrets {};
        for (const auto legal_input : legal_inputs)
        {
            regrets.emplace(legal_input, 0.0);
        }

        return regrets;
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
    {
        if (propnet.is_game_over(root_state))
        {
            throw std::logic_error {"Can't do CFR on a terminal state"};
        }
    }

    std::vector<std::unordered_map<std::uint32_t, double>> MCCfr::search()
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
                auto state {root_state};
                make_traversers_move(current_information_sets, traversing_role, state);
            }
        }

        std::vector<std::unordered_map<std::uint32_t, double>> policies {};
        std::transform(
            base_information_sets.begin(),
            base_information_sets.end(),
            std::back_inserter(policies),
            [](auto& entry)
            {
                auto& cumulative_policy {entry.second.cumulative_policy};
                for (auto& [input, probability] : cumulative_policy)
                {
                    probability /= NUM_ITERATIONS;
                }

                return std::move(cumulative_policy);
            }
        );

        return policies;
    }

    double MCCfr::make_traversers_move(
        std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>>& current_information_sets,
        propnet::Role& traversing_role,
        propnet::State& state
    )
    {
        /*
        Check if this is a terminal state
        */
        if (propnet.is_game_over(state))
        {
            /*
            Return the traversers utility here
            */
            return traversing_role.get_reward(state);
        }

        const auto id {traversing_role.get_id()};
        auto& current_information_set {current_information_sets.at(id).get()};

        std::unordered_map<std::uint32_t, std::uint32_t> rewards {};
        double policy_reward {0.0};
        const auto regret_matched_policy {current_information_set.regret_match()};
        for (const auto& [input, probability] : regret_matched_policy)
        {
            current_information_set.choose_input(input);
            auto state_copy {state};
            const auto reward {make_non_traversers_moves(current_information_sets, traversing_role, state_copy)};
            rewards.emplace(input, reward);
            policy_reward += probability * reward;
        }

        for (auto& [input, regret] : current_information_set.regrets)
        {
            const auto current_regret {rewards.at(input) - policy_reward};
            regret += current_regret;
        }

        return policy_reward;
    }

    double MCCfr::make_non_traversers_moves(
        std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>>& current_information_sets,
        propnet::Role& traversing_role,
        propnet::State& state
    )
    {
        for (const auto& player_role : player_roles)
        {
            /*
            Choose a move for each role according to the regret matching
            */
            const auto id {player_role.get_id()};
            auto& current_information_set {current_information_sets.at(id).get()};
            const auto regret_matched_policy {current_information_set.regret_match()};

            /*
            Update strategy
            */
            for (auto& [input, probability] : current_information_set.cumulative_policy)
            {
                probability += regret_matched_policy.at(input);
            }
        }

        return make_randoms_move(current_information_sets, traversing_role, state);
    }

    double MCCfr::make_randoms_move(
        std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>>& current_information_sets,
        propnet::Role& traversing_role,
        propnet::State& current_state
    )
    {}

    std::int32_t MCCfr::make_non_traversers_moves(
        std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>>& current_information_sets,
        propnet::Role& traversing_role,
        std::vector<propnet::Role>::iterator acting_role_it,
        propnet::State& state
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

            /*
            Make moves and take observations
            */
            propnet.take_sees_inputs(state, inputs);
            std::vector<std::vector<bool>> all_observations {};
            for (const auto& role : player_roles)
            {
                all_observations.push_back(role.get_observations(state));
            }
            propnet.take_non_sees_inputs(state, inputs);

            /*
            Check if this is a terminal state
            */
            if (propnet.is_game_over(state))
            {
                /*
                Return the traversers utility here
                */
                return traversing_role.get_reward(state);
            }

            auto all_observations_it {all_observations.begin()};
            std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>> next_information_sets {};
            for (const auto& role : player_roles)
            {
                const auto id {role.get_id()};
                auto legal_inputs {role.get_legal_inputs(state)};

                auto& current_information_set {current_information_sets.at(id)};

                auto& next_information_set {current_information_set.get().get_next_information_set(*all_observations_it, std::move(legal_inputs))};
                next_information_sets.emplace(id, next_information_set);
                ++all_observations_it;
            }

            acting_role_it = player_roles.begin();
            return search_impl(next_information_sets, traversing_role, acting_role_it, state);
        }

        // Do regret matching, whatever that means
        // 6:    σ(I) ← RegretMatching(rI )

        if (traversing_role.get_id() == acting_role_it->get_id())
        {
            // 8:      Let u be an array indexed by actions and uσ ← 0
            // 9:      for a ∈ A(I) do
            // 10:       u[a] ← ExternalSampling(ha, i)
            // 11:       uσ ← uσ + σ(I, a) · u[a]
            // 12:     for a ∈ A(I) do
            // 13:       By Equation 4.20, compute r˜(I, a) ← u[a] − uσ
            // 14:       rI [a] ← rI [a] + ˜r(I, a)
            // 15:     return uσ
        }
        else
        {
            // 17:     Sample action a0 from σ(I)
            // 18:     u ← ExternalSampling(ha0, i)
            // 19:     for a ∈ A(I) do
            // 20:       sI [a] ← sI [a] + σ(I, a)
            // 21:     return u
        }

        return 0;
    }
}
