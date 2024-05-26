#pragma once

#include "../nodes.hpp"

#include <vector>
#include <string_view>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <span>
#include <memory>

namespace propnet
{
    class Role
    {
        public:
            struct See
            {
                std::shared_ptr<const PropositionNode> node;
            };

            struct Legal
            {
                std::shared_ptr<const PropositionNode> node;
                std::uint32_t input;
            };

            struct Goal
            {
                std::shared_ptr<const PropositionNode> node;
                std::uint32_t value;
            };

            Role(std::string_view name, const std::vector<See>& sees, const std::vector<Legal>& legals, const std::vector<Goal>& goals);

            std::string_view get_name() const;
            std::uint32_t get_reward(const State& state) const;
            void take_observations(const State& state);
            const std::vector<bool>& get_observations() const;
            void print_observations() const;
            void take_legal_inputs(const State& state);
            std::span<const std::uint32_t> get_legal_inputs();
            void print_legals() const;
        private:
            const std::unordered_set<std::uint32_t> EMPTY_INPUTS {};

            std::string name;
            std::vector<See> sees;
            std::vector<Legal> legals;
            std::vector<Goal> goals;
            std::vector<bool> sees_cache;
            std::vector<std::uint32_t> inputs_cache;
            std::size_t inputs_cache_size;
    };
}
