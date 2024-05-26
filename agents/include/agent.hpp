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
            void take_observations(const propnet::State& state);
            std::uint32_t get_input(const propnet::State& state);
        private:
            virtual std::uint32_t get_legal(const std::vector<bool>& sees, std::span<const std::uint32_t> legals) = 0;

            propnet::Role role;
            const propnet::Propnet& propnet;
    };
}
