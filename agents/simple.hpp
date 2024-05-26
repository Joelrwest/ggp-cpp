#pragma once

#include "include/agent.hpp"

#include <random>

namespace agents
{
    class SimpleAgent : public Agent
    {
        public:
            SimpleAgent(const propnet::Role& role, const propnet::Propnet& propnet);

            std::vector<std::uint32_t> get_legal_inputs(const propnet::State& state) const;
        private:
            virtual void get_legal_inputs_impl(std::span<std::uint32_t> legal_inputs) const = 0;

            const propnet::Role& role;
    };
}
