#pragma once

#include "../../propnet/include/propnet.hpp"
#include "../../propnet/include/role.hpp"

#include <vector>
#include <cstdint>
#include <span>

namespace agents {
    class Agent
    {
        public:
            Agent(const propnet::Role& role, const propnet::Propnet& propnet);

            std::uint32_t get_reward(const propnet::State& state) const;
            virtual void prepare_new_game();
            virtual void take_observations(const propnet::State& state);
            std::uint32_t get_legal_input(const propnet::State& state);
            const std::vector<bool>& get_observations_cache() const;
        private:
            virtual std::uint32_t get_legal_input_impl(std::span<const std::uint32_t> legal_inputs) = 0;

            const propnet::Role& role;
            const propnet::Propnet& propnet;
            std::vector<bool> observations_cache {};
    };
}
