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
            InformationSet();

            InformationSet& get_next_information_set(const std::vector<bool>& observations);
            void choose_input(std::uint32_t input);
            std::uint32_t get_chosen_input() const;
            const std::vector<double>& get_policy() const;
        private:
            std::vector<double> policy;
            std::vector<double> regrets;
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

            std::vector<std::vector<double>> search();
        private:
            static constexpr std::size_t NUM_ITERATIONS {1000};

            std::int32_t search_impl(
                std::unordered_map<propnet::Role::Id, std::reference_wrapper<InformationSet>>& current_information_sets,
                propnet::Role& traversing_role,
                std::vector<propnet::Role>::iterator acting_role_it,
                propnet::State current_state
            );

            const propnet::Propnet& propnet;
            std::vector<propnet::Role> player_roles {};
            std::optional<propnet::Role> random_role {};
            propnet::State root_state;
            std::unordered_map<propnet::Role::Id, InformationSet> base_information_sets;
    };
}
