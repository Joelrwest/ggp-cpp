#pragma once

#include "../include/agent.hpp"

#include <random>

namespace agents
{
    class SimpleAgent : public Agent
    {
        public:
            SimpleAgent(const propnet::Role& role);

            std::vector<propnet::PropId> get_legal_inputs(const propnet::State& state) const;
        private:
            virtual void get_legal_inputs_impl(std::span<propnet::PropId> legal_inputs) const = 0;

            const propnet::Role& role;
    };
}
