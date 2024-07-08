#pragma once

#include "../src/nodes.hpp"

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

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

    Role(std::string_view name, std::span<const std::shared_ptr<const PropositionNode>> sees,
         std::span<const Legal> legals, std::span<const Goal> goals);

    std::string_view get_name() const;
    Reward get_reward(const State &state) const;
    std::vector<bool> get_observations(const State &state) const;
    void print_observations(const std::vector<bool> &observations) const;
    std::vector<PropId> get_legal_inputs(const State &state) const;
    void print_legal_inputs(std::span<const PropId> inputs) const;
    std::size_t get_max_policy_size() const;
    Id get_id() const;

  private:
    static constexpr auto MAX_DISPLAYED_OBSERVATIONS{20};

    static Id allocate_role_id();

    const InputSet EMPTY_INPUTS{};

    std::string name;
    std::vector<std::shared_ptr<const PropositionNode>> sees;
    std::vector<Legal> legals;
    std::vector<Goal> goals;
    Id role_id;
};
} // namespace propnet
