#include "cfr.hpp"

namespace rebel
{
    InformationSet::InformationSet(const std::vector<std::uint32_t>& legal_inputs) :
        is_choice {legal_inputs.size() > 1},
        cumulative_policy {make_zeroed_map(legal_inputs)},
        regrets {is_choice ? make_zeroed_map(legal_inputs) : std::unordered_map<uint32_t, double> {}},
        next_information_sets {},
        previous_input {std::nullopt}
    {}

    InformationSet& InformationSet::get_next_information_set(const std::vector<bool>& observations, const propnet::Role& player_role, const propnet::State& state)
    {
        auto& next_information_set_ptr {next_information_sets[get_chosen_input()][observations]};
        if (next_information_set_ptr == nullptr)
        {
            const auto legal_inputs {player_role.get_legal_inputs(state)};
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
            [](auto accumulation, const auto& regret_pair)
            {
                return accumulation + std::max(regret_pair.second, 0.0);
            }
        )};

        std::unordered_map<std::uint32_t, double> policy {};
        if (total_positive_regret > 0.0)
        {
            for (const auto& [input, regret] : regrets)
            {
                const auto probability {regret > 0.0 ? regret / total_positive_regret : 0.0};
                policy.emplace(input, probability);
            }
        }
        else
        {
            const auto num_inputs {regrets.size()};
            const auto uniform_probability {1.0 / num_inputs};
            for (const auto& [input, _] : regrets)
            {
                policy.emplace(input, uniform_probability);
            }
        }

        return policy;
    }

    propnet::State InformationSet::get_sampled_state() const
    {
        return misc::sample_random(sampled_states);
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

    MCCfr::MCCfr(const propnet::Propnet& propnet) :
        propnet {propnet},
        player_roles {propnet.get_player_roles()},
        random_role {propnet.get_random_role()}
    {}

    std::vector<std::unordered_map<std::uint32_t, double>> MCCfr::search(const propnet::State& state)
    {
        /*
        Psuedocode from:

        MONTE CARLO SAMPLING AND REGRET MINIMIZATION FOR EQUILIBRIUM
        COMPUTATION AND DECISION-MAKING IN LARGE EXTENSIVE FORM GAMES

        TODO: Remember to add pruning
        */
        auto base_information_sets {create_base_information_sets(state)};
        std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>> current_information_sets {};
        for (auto& [role_id, base_information_set] : base_information_sets)
        {
            current_information_sets.emplace(role_id, base_information_set);
        }

        for (std::size_t iteration_count {0}; iteration_count < NUM_ITERATIONS; ++iteration_count)
        {
            /*
            The role that's currently traversing
            */
            for (auto& traversing_role : player_roles)
            {
                std::cout << iteration_count << '\n';
                auto state_copy {state};
                make_traversers_move(current_information_sets, traversing_role, state_copy);
            }
        }

        const auto num_players {propnet.num_player_roles()};
        std::vector<std::unordered_map<std::uint32_t, double>> policies {};
        std::transform(
            base_information_sets.begin(),
            base_information_sets.end(),
            std::back_inserter(policies),
            [num_players](auto& entry)
            {
                auto& cumulative_policy {entry.second.cumulative_policy};
                const auto cumulative_policy_sum {std::accumulate(
                    cumulative_policy.begin(),
                    cumulative_policy.end(),
                    0.0,
                    [](const auto accumulation, const auto pair)
                    {
                        return accumulation + pair.second;
                    }
                )};

                if (cumulative_policy_sum == 0.0)
                {
                    throw std::logic_error {"Nothing recorded in cumulative policy"};
                }

                for (auto& [input, probability] : cumulative_policy)
                {
                    probability /= cumulative_policy_sum;
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
            if (id == traversing_role.get_id())
            {
                continue;
            }

            auto& current_information_set {current_information_sets.at(id).get()};
            const auto regret_matched_policy {current_information_set.regret_match()};
            const auto chosen_input {misc::sample_policy(regret_matched_policy)};
            current_information_set.choose_input(chosen_input);

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
        propnet::State& state
    )
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

            auto& current_information_set {current_information_sets.at(id).get()};

            auto& next_information_set {current_information_set.get_next_information_set(*all_observations_it, role, state)};
            next_information_sets.emplace(id, next_information_set);
            ++all_observations_it;
        }

        return make_traversers_move(next_information_sets, traversing_role, state);
    }

    std::unordered_map<propnet::Role::Id, InformationSet> MCCfr::create_base_information_sets(const propnet::State& state)
    {
        std::unordered_map<propnet::Role::Id, InformationSet> base_information_sets {};
        for (const auto& role : propnet.get_player_roles())
        {
            const auto id {role.get_id()};

            const auto legal_inputs {role.get_legal_inputs(state)};
            base_information_sets.emplace(id, std::move(legal_inputs));
        }

        return base_information_sets;
    }
}
