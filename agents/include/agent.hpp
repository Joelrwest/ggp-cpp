#pragma once

#include "../../propnet/include/propnet.hpp"
#include "../../propnet/include/role.hpp"

#include <optional>
#include <span>
#include <vector>

namespace agents
{
class Agent
{
  public:
    Agent(const propnet::Role &role);

    propnet::Reward get_reward(const propnet::State &state) const;
    virtual void prepare_new_game();
    virtual void take_observations(const propnet::State &state);
    propnet::PropId get_legal_input(const propnet::State &state);
    const std::vector<bool> &get_observations_cache() const;
    std::string_view get_role_name() const;

  protected:
    virtual void add_history(propnet::PropId prev_input);

  private:
    virtual propnet::PropId get_legal_input_impl(std::span<const propnet::PropId> legal_inputs) = 0;

    std::optional<propnet::PropId> prev_input{};
    const propnet::Role &role;
    std::vector<bool> observations_cache{};
};
} // namespace agents
