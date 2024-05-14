#pragma once

#include "include/agent.hpp"

#include <random>

namespace agents {
    class RandomAgent : public Agent
    {
        public:
            RandomAgent(const propnet::Role& role, const propnet::Propnet& propnet);
        protected:
            std::uint32_t get_legal(const std::vector<bool>& sees, std::span<const std::uint32_t> legals) override;
        private:
            static std::mt19937 random_engine (std::random_device());
    };

    class FirstAgent : public Agent
    {
        public:
            FirstAgent(const propnet::Role& role, const propnet::Propnet& propnet);
        protected:
            std::uint32_t get_legal(const std::vector<bool>& sees, std::span<const std::uint32_t> legals) override;
    };

    class LastAgent : public Agent
    {
        public:
            LastAgent(const propnet::Role& role, const propnet::Propnet& propnet);
        protected:
            std::uint32_t get_legal(const std::vector<bool>& sees, std::span<const std::uint32_t> legals) override;
    };
};
