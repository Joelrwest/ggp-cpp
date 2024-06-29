#pragma once

#include "../../propnet/include/propnet.hpp"
#include "../../agents/include/non_seeing.hpp" // TODO
#include "misc.hpp"

#include <lru_cache_policy.hpp>
#include <vector>
#include <cstdint>
#include <memory>

namespace rebel
{
    class InformationSet
    {
        public:
            InformationSet(const std::vector<std::uint32_t>& legal_inputs);

            InformationSet& get_next_information_set(const std::vector<bool>& observations, std::vector<std::uint32_t>&& legal_inputs);
            void choose_input(std::uint32_t input);
            std::uint32_t get_chosen_input() const;
            std::unordered_map<std::uint32_t, double> regret_match() const;

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
            MCCfr(const propnet::Propnet& propnet, const propnet::State& root_state);

            std::vector<std::unordered_map<std::uint32_t, double>> search();
        private:
            static constexpr std::size_t NUM_ITERATIONS {1000};

            double make_randoms_move(
                std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>>& current_information_sets,
                propnet::Role& traversing_role,
                propnet::State& state
            );

            double make_traversers_move(
                std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>>& current_information_sets,
                propnet::Role& traversing_role,
                propnet::State& state
            );

            double make_non_traversers_moves(
                std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>>& current_information_sets,
                propnet::Role& traversing_role,
                propnet::State& state
            );

            const propnet::Propnet& propnet;
            std::vector<propnet::Role> player_roles {};
            std::optional<propnet::Role> random_role {};
            propnet::State root_state;
            std::unordered_map<propnet::Role::Id, InformationSet> base_information_sets;
    };
}
