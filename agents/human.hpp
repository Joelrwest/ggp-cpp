#pragma once

#include "agent.hpp"

namespace agents {
    class HumanAgent : public Agent
    {
        public:
            HumanAgent(const propnet::Role& role);

            static constexpr auto NAME {"human"};
        private:
            std::uint32_t get_legal_input_impl(std::span<const std::uint32_t> legal_inputs) override;

            const propnet::Role& role;
    };
}
