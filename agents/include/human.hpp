#pragma once

#include "agent.hpp"

namespace agents {
    class HumanAgent : public Agent
    {
        public:
            HumanAgent(const propnet::Role& role);

            static constexpr auto NAME {"human"};
        private:
            propnet::PropId get_legal_input_impl(std::span<const propnet::PropId> legal_inputs) override;

            const propnet::Role& role;
    };
}
