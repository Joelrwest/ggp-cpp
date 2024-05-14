#pragma once

#include "agent.hpp"

namespace agents {
    class HumanAgent : public Agent
    {
        public:
            HumanAgent(const propnet::Role& role, const propnet::Propnet& propnet);

            static constexpr auto NAME {"human"};
        private:
            std::uint32_t get_legal(const std::vector<bool>& sees, std::span<const std::uint32_t> legals) override;

            const propnet::Propnet& propnet;
    };
};
