#pragma once

#include "../../propnet/include/propnet.hpp"
#include "../../agents/include/non_seeing.hpp" // TODO
#include "misc.hpp"
#include "random_sampler.hpp"

#include <lru_cache_policy.hpp>
#include <vector>
#include <cstdint>
#include <memory>

namespace rebel::search
{
    class InformationSet
    {
        public:
            InformationSet(const std::vector<std::uint32_t>& legal_inputs);

            InformationSet& get_next_information_set(const std::vector<bool>& observations, const propnet::Role& player_role, const propnet::State& state);
            void choose_input(std::uint32_t input);
            std::uint32_t get_chosen_input() const;
            std::unordered_map<std::uint32_t, double> regret_match() const;

            std::unordered_map<std::uint32_t, double> cumulative_policy; // TODO: I know it's bad style to have this public but meh
            std::unordered_map<std::uint32_t, double> regrets;
        private:
            static std::unordered_map<std::uint32_t, double> make_zeroed_map(const std::vector<std::uint32_t>& legal_inputs);

            std::unordered_map<
                std::uint32_t,
                std::unordered_map<std::vector<bool>, std::unique_ptr<InformationSet>>
            > next_information_sets;
            std::optional<std::uint32_t> previous_input;
    };

    class ExternalSamplingMCCFR
    {
        public:
            ExternalSamplingMCCFR(const propnet::Propnet& propnet);

            std::vector<std::unordered_map<std::uint32_t, double>> search(const propnet::State& state);
        private:
            static constexpr std::size_t NUM_ITERATIONS {10000};
            static constexpr std::size_t PRINT_FREQUENCY {NUM_ITERATIONS / 25};

            double make_traversers_move(
                std::vector<std::reference_wrapper<InformationSet>>& current_information_sets,
                propnet::Role& traversing_role,
                propnet::State& state
            );

            double make_non_traversers_moves(
                std::vector<std::reference_wrapper<InformationSet>>& current_information_sets,
                propnet::Role& traversing_role,
                propnet::State& state
            );

            double next_state(
                std::vector<std::reference_wrapper<InformationSet>>& current_information_sets,
                propnet::Role& traversing_role,
                propnet::State& state
            );

            static std::vector<InformationSet> create_base_information_sets(const propnet::Propnet& propnet);

            const propnet::Propnet& propnet;
            std::vector<propnet::Role> player_roles;
            std::optional<propnet::Role> random_role;
            std::vector<InformationSet> base_information_sets;
    };
}
