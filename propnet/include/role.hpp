#pragma once

#include "../src/nodes.hpp"

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
            using Id = std::uint16_t;

            struct Legal
            {
                std::shared_ptr<const PropositionNode> node;
                PropId legal;
                PropId input;
            };

            struct Goal
            {
                std::shared_ptr<const PropositionNode> node;
                Reward value;
            };

            Role(std::string_view name, const std::vector<std::shared_ptr<const PropositionNode>>& sees, const std::vector<Legal>& legals, const std::vector<Goal>& goals);

            std::string_view get_name() const;
            Reward get_reward(const State& state) const;
            std::vector<bool> get_observations(const State& state) const;
            void print_observations(const std::vector<bool>& observations) const;
            std::vector<PropId> get_legal_inputs(const State& state) const;
            void print_legal_inputs(std::span<const PropId> inputs) const;
            std::size_t get_max_policy_size() const;
            Id get_id() const;
        private:
            static constexpr auto MAX_DISPLAYED_OBSERVATIONS {20};

            static Id allocate_role_id();

            const InputSet EMPTY_INPUTS {};

            std::string name;
            std::vector<std::shared_ptr<const PropositionNode>> sees;
            std::vector<Legal> legals;
            std::vector<Goal> goals;
            Id role_id;
    };
}
