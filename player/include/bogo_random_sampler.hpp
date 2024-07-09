#pragma once

#include "../../propnet/include/propnet.hpp"

#include <list>
#include <optional>

namespace player
{
class BogoRandomSampler
{
  public:
    BogoRandomSampler(const propnet::Role &sampler_role, const propnet::Propnet &propnet);

    void prepare_new_game();
    void add_history(const std::vector<bool> &observation, propnet::PropId prev_input);
    propnet::State sample_state();

  private:
    struct History
    {
        std::vector<bool> observation;
        propnet::PropId prev_input;
    };

    std::optional<propnet::State> sample_state_impl();

    std::list<History> all_histories{};
    const propnet::Propnet &propnet;
    const propnet::Role sampler_role;
    std::vector<propnet::Role> player_roles;
    std::optional<propnet::Role> random_role;
};
} // namespace player
