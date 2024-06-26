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

            InformationSet& get_next_information_sets(std::uint32_t previous_input, const std::vector<bool>& observations);
            const std::vector<double>& get_policy() const;
        private:
            std::vector<double> policy;
            std::vector<double> regrets;
            std::unordered_map<
                std::uint32_t,
                std::unordered_map<std::vector<bool>, std::unique_ptr<InformationSet>>
            > next_information_sets;
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
            misc::Cache<propnet::State, std::vector<double>, caches::LRUCachePolicy, STATE_CACHE_SIZE> state_cache; // TODO
    };

    class MCCfr
    {
        public:
            MCCfr(const propnet::Propnet& propnet, const propnet::State& root_state);

            std::vector<std::vector<double>> search();
        private:
            static constexpr std::size_t NUM_ITERATIONS {100};

            struct RoleInformationSet
            {
                RoleInformationSet(const propnet::Role& role, InformationSet& information_set);

                propnet::Role role;
                std::reference_wrapper<InformationSet> information_set;
            };

            std::uint32_t search_impl(std::vector<RoleInformationSet>& role_information_sets);

            const propnet::Propnet& propnet;
            propnet::State root_state;
            std::vector<InformationSet> base_information_sets;
    };
}
