#pragma once

#include "agent.hpp"

#include <random>

namespace agents
{
    class RandomAgent : public Agent
    {
        public:
            RandomAgent(const propnet::Role& role, const propnet::Propnet& propnet);

            static constexpr auto NAME {"random"};
        private:
            std::uint32_t get_legal(const std::vector<bool>& sees, std::span<const std::uint32_t> legals) override;
    };

    class FirstAgent : public Agent
    {
        public:
            FirstAgent(const propnet::Role& role, const propnet::Propnet& propnet);

            static constexpr auto NAME {"first"};
        private:
            std::uint32_t get_legal(const std::vector<bool>& sees, std::span<const std::uint32_t> legals) override;
    };

    class LastAgent : public Agent
    {
        public:
            LastAgent(const propnet::Role& role, const propnet::Propnet& propnet);

            static constexpr auto NAME {"last"};
        private:
            std::uint32_t get_legal(const std::vector<bool>& sees, std::span<const std::uint32_t> legals) override;
    };
}
