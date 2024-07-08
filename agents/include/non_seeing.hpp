#pragma once

#include "../src/simple.hpp"

#include <random>

namespace agents
{
class NonSeeingAgent : public SimpleAgent
{
  public:
    NonSeeingAgent(const propnet::Role &role);

    void take_observations(const propnet::State &state) override;
};

class RandomAgent : public NonSeeingAgent
{
  public:
    RandomAgent(const propnet::Role &role);

    static constexpr auto NAME{"random"};

  private:
    propnet::PropId get_legal_input_impl(std::span<const propnet::PropId> legal_inputs) override;
    void get_legal_inputs_impl(std::span<propnet::PropId> legal_inputs) const override;
};

class FirstAgent : public NonSeeingAgent
{
  public:
    FirstAgent(const propnet::Role &role);

    static constexpr auto NAME{"first"};

  private:
    propnet::PropId get_legal_input_impl(std::span<const propnet::PropId> legal_inputs) override;
    void get_legal_inputs_impl(std::span<propnet::PropId> legal_inputs) const override;
};

class LastAgent : public NonSeeingAgent
{
  public:
    LastAgent(const propnet::Role &role);

    static constexpr auto NAME{"last"};

  private:
    propnet::PropId get_legal_input_impl(std::span<const propnet::PropId> legal_inputs) override;
    void get_legal_inputs_impl(std::span<propnet::PropId> legal_inputs) const override;
};
} // namespace agents
