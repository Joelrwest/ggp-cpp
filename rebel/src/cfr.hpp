#pragma once

#include "../../propnet/include/propnet.hpp"
#include "../../agents/include/non_seeing.hpp" // TODO
#include "misc.hpp"

#include <lru_cache_policy.hpp>
#include <vector>
#include <cstdint>

namespace rebel
{
    class Cfr
    {
        public:
            Cfr(const propnet::Role& cfr_role, const propnet::Propnet& propnet, std::span<const std::uint32_t> legal_inputs, propnet::State root_state);

            std::vector<double> search();
        private:
            static constexpr auto MAX_DEPTH {20};
            static constexpr auto STATE_CACHE_SIZE {2000};

            std::vector<double> search(std::uint32_t depth);

            const propnet::Role& cfr_role;
            const propnet::Propnet& propnet;
            std::span<const std::uint32_t> legal_inputs;
            propnet::State root_state;
            std::vector<agents::RandomAgent> player_agents;
            std::optional<agents::RandomAgent> random_agent;
            misc::Cache<propnet::State, std::vector<double>, caches::LRUCachePolicy, STATE_CACHE_SIZE> state_cache; // TODO
    };
}
