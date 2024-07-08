#pragma once

#include "../../propnet/include/propnet.hpp"
#include "misc.hpp"
#include "random_sampler.hpp"
#include "types.hpp"

#include <lru_cache_policy.hpp>
#include <memory>
#include <vector>

namespace rebel::search
{
class InformationSet
{
  public:
    InformationSet(std::span<const propnet::PropId> legal_inputs);

    InformationSet &get_next_information_set(const std::vector<bool> &observations, const propnet::Role &player_role,
                                             const propnet::State &state);
    void choose_input(propnet::PropId input);
    propnet::PropId get_chosen_input() const;
    Regrets regret_match() const;

    Policy cumulative_policy; // TODO: I know it's bad style to have this public but meh
    Regrets regrets;
    ExpectedValue cumulative_expected_value;
    std::size_t total_visits;

  private:
    template <typename T> static T make_zeroed_map(std::span<const propnet::PropId> legal_inputs);

    std::unordered_map<propnet::PropId, std::unordered_map<std::vector<bool>, std::unique_ptr<InformationSet>>>
        next_information_sets;
    std::optional<propnet::PropId> previous_input;
};

template <typename T> T InformationSet::make_zeroed_map(std::span<const propnet::PropId> legal_inputs)
{
    T map{};
    for (const auto legal_input : legal_inputs)
    {
        map.emplace(legal_input, 0.0);
    }

    return map;
}

class ExternalSamplingMCCFR
{
  public:
    ExternalSamplingMCCFR(const propnet::Propnet &propnet);

    std::vector<std::pair<Policy, ExpectedValue>> search(const propnet::State &state);

  private:
    static constexpr std::size_t NUM_ITERATIONS{10000000};
    static constexpr std::size_t PRINT_FREQUENCY{NUM_ITERATIONS / 5};

    ExpectedValue make_traversers_move(std::vector<std::reference_wrapper<InformationSet>> &current_information_sets,
                                       propnet::Role &traversing_role, propnet::State &state);

    ExpectedValue make_non_traversers_moves(
        std::vector<std::reference_wrapper<InformationSet>> &current_information_sets, propnet::Role &traversing_role,
        propnet::State &state);

    ExpectedValue next_state(std::vector<std::reference_wrapper<InformationSet>> &current_information_sets,
                             propnet::Role &traversing_role, propnet::State &state);

    static std::vector<InformationSet> create_base_information_sets(const propnet::Propnet &propnet);

    const propnet::Propnet &propnet;
    std::vector<propnet::Role> player_roles;
    std::optional<propnet::Role> random_role;
    std::vector<InformationSet> base_information_sets;
};
} // namespace rebel::search
