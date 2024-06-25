#pragma once

#include "../../propnet/include/propnet.hpp"
#include "../../agents/include/non_seeing.hpp" // TODO

#include <optional>
#include <list>

namespace rebel
{
    class NaiveSampler
    {
        public:
            NaiveSampler(const propnet::Role& sampler_role, const propnet::Propnet& propnet);

            void prepare_new_game();
            void add_history(const std::vector<bool>& observation, std::uint32_t prev_input);
            propnet::State sample_state();
        private:
            struct History
            {
                std::vector<bool> observation;
                std::uint32_t prev_input;
            };

            std::list<History> all_histories {};
            const propnet::Propnet& propnet;
            const propnet::Role& sampler_role;
            std::vector<propnet::Role> player_roles;
            std::optional<propnet::Role> random_role;

            using AllHistories = decltype(all_histories);

            std::optional<propnet::State> sample_state_impl(AllHistories::const_iterator all_histories_it, AllHistories::const_iterator all_histories_end_it, propnet::State state);
    };
}
