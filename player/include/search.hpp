#pragma once

#include "../../propnet/include/propnet.hpp"
#include "misc.hpp"
#include "model.hpp"
#include "random_sampler.hpp"
#include "types.hpp"

#include <chrono>
#include <lru_cache_policy.hpp>
#include <memory>
#include <vector>

namespace player::search
{
class InformationSet
{
  public:
    InformationSet(std::span<const propnet::PropId> legal_inputs);

    InformationSet &get_next_information_set(const std::vector<bool> &observations, const propnet::Role &player_role,
                                             const propnet::State &state);
    void choose_input(propnet::PropId input);
    propnet::PropId get_chosen_input() const;
    Policy regret_match() const;
    std::pair<Policy, ExpectedValue> normalise() const;

    Policy cumulative_policy; // TODO: I know it's bad style to have this public but meh
    Regrets regrets;
    ExpectedValue cumulative_expected_value;
    std::size_t total_visits;

  private:
    std::unordered_map<propnet::PropId, std::unordered_map<std::vector<bool>, std::unique_ptr<InformationSet>>>
        next_information_sets;
    std::optional<propnet::PropId> previous_input;
};

class MCCFR
{
  public:
    class Options
    {
      public:
        using LoggerFunction = std::function<void(const std::vector<std::reference_wrapper<InformationSet>> &)>;

        Options();

        Options &add_iteration_limit(std::size_t num_iterations);
        Options &add_time_limit(std::chrono::seconds duration);
        Options &add_logger(LoggerFunction &&logger);

        std::size_t get_iteration_limit() const;
        std::function<bool()> get_time_remaining_function() const;
        LoggerFunction &get_logger();

      private:
        inline static const auto default_logger{[](const auto &) {}};
        inline static const auto always_time_remaining_function{[]() { return true; }};

        std::size_t iteration_limit;
        std::optional<std::chrono::seconds> time_limit;
        LoggerFunction logger;
    };

    std::vector<std::pair<Policy, ExpectedValue>> search(const propnet::State &state);
    std::vector<std::pair<Policy, ExpectedValue>> search(const propnet::State &state, Options &options);

  protected:
    MCCFR(const propnet::Propnet &propnet, std::optional<std::reference_wrapper<Model>> model, Depth depth_limit);

  private:
    static constexpr auto DEFAULT_MAX_ITERATIONS{static_cast<std::size_t>(1e5)};

    ExpectedValue make_traversers_move(std::vector<std::reference_wrapper<InformationSet>> &current_information_sets,
                                       propnet::Role &traversing_role, propnet::State &state, Depth curr_depth);

    ExpectedValue make_non_traversers_moves(
        std::vector<std::reference_wrapper<InformationSet>> &current_information_sets, propnet::Role &traversing_role,
        propnet::State &state, Depth curr_depth);

    ExpectedValue next_state(std::vector<std::reference_wrapper<InformationSet>> &current_information_sets,
                             propnet::Role &traversing_role, propnet::State &state, Depth curr_depth);

    static std::vector<InformationSet> create_base_information_sets(const propnet::Propnet &propnet);

    const propnet::Propnet &propnet;
    std::vector<propnet::Role> player_roles;
    std::optional<propnet::Role> random_role;
    std::vector<InformationSet> base_information_sets;
    std::optional<std::reference_wrapper<Model>> model;
    Depth depth_limit;
};

class FullMCCFR : public MCCFR
{
  public:
    FullMCCFR(const propnet::Propnet &propnet);
};

class DepthLimitedMCCFR : public MCCFR
{
  public:
    DepthLimitedMCCFR(const propnet::Propnet &propnet, Model &model, Depth depth_limit);
};
} // namespace player::search
