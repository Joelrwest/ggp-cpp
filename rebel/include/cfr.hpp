#pragma once

#include "../../propnet/include/propnet.hpp"
#include "../../agents/include/non_seeing.hpp" // TODO
#include "../src/misc.hpp"
#include "training_sampler.hpp"

#include <lru_cache_policy.hpp>
#include <vector>
#include <cstdint>
#include <memory>

namespace rebel
{
    class InformationSet
    {
        public:
            InformationSet(std::vector<std::uint32_t>&& legal_inputs, std::vector<propnet::State>&& sampled_states);

            InformationSet& get_next_information_set(const std::vector<bool>& observations, const propnet::Role& player_role, const propnet::State& state);
            void choose_input(std::uint32_t input);
            std::uint32_t get_chosen_input() const;
            std::unordered_map<std::uint32_t, double> regret_match() const;
            propnet::State get_sampled_state() const;

            std::unordered_map<std::uint32_t, double> cumulative_policy; // TODO: I know it's bad style to have this public but meh
            std::unordered_map<std::uint32_t, double> regrets;
        private:
            static std::unordered_map<std::uint32_t, double> make_zeroed_map(const std::vector<std::uint32_t>& legal_inputs);

            bool is_choice; // TODO: Just to stop ourselves from doing too much if there's no choice
            std::unordered_map<
                std::uint32_t,
                std::unordered_map<std::vector<bool>, std::unique_ptr<InformationSet>>
            > next_information_sets;
            std::optional<std::uint32_t> previous_input;
            std::vector<propnet::State> sampled_states;
    };

    class VanillaCfr
    {
        public:
            VanillaCfr(const propnet::Role& cfr_role, const propnet::Propnet& propnet, std::span<const std::uint32_t> legal_inputs, propnet::State root_state);

            std::vector<double> search();
        private:
            static constexpr auto MAX_DEPTH {20};
            static constexpr auto STATE_CACHE_SIZE {2000};

            std::vector<double> search(std::uint32_t depth);

            const propnet::Role& cfr_role;
            const propnet::Propnet& propnet;
            std::span<const std::uint32_t> legal_inputs;
            propnet::State root_state;
            std::vector<propnet::Role> player_roles;
            std::optional<propnet::Role> random_role;
    };

    class MCCfr
    {
        public:
            MCCfr(const propnet::Propnet& propnet);

            void add_history(std::uint32_t role_id, const std::vector<bool>& observation, std::uint32_t prev_input);
            std::vector<std::unordered_map<std::uint32_t, double>> search();
        private:
            static constexpr std::size_t NUM_ITERATIONS {10000};
            static constexpr auto NUM_SAMPLED_STATES {20};

            double make_traversers_move(
                std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>>& current_information_sets,
                propnet::Role& traversing_role
            );

            double make_non_traversers_moves(
                std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>>& current_information_sets,
                propnet::Role& traversing_role
            );

            double make_randoms_move(
                std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>>& current_information_sets,
                propnet::Role& traversing_role
            );

            std::unordered_map<propnet::Role::Id, InformationSet> create_base_information_sets();

            const propnet::Propnet& propnet;
            std::vector<propnet::Role> player_roles;
            std::optional<propnet::Role> random_role;
            std::vector<TrainingSampler> samplers;
            std::unordered_map<propnet::Role::Id, InformationSet> base_information_sets;
    };
}
