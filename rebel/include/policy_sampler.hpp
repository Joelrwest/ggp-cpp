#pragma once

#include "../../propnet/include/propnet.hpp"

#include <tuple>
#include <shared_mutex>
#include <deque>

namespace rebel
{
    class PolicySampler
    {
        public:
            PolicySampler(const propnet::Role& sampler_role, const propnet::Propnet& propnet);

            void prepare_new_game();
            void add_history(const std::vector<bool>& observation, std::uint32_t prev_input);
            void pop_history();
            propnet::State sample_state();
            std::vector<propnet::State> sample_states(std::size_t num_states);
        private:
            struct History
            {
                History(const std::vector<bool>& observation, std::uint32_t prev_input);
                History(const History& other) = delete;
                History& operator=(const History&) = delete;

                std::vector<bool> observation;
                std::uint32_t prev_input;
                std::unordered_set<propnet::State> invalid_state_cache;
                std::shared_mutex invalid_state_cache_lock;
                std::unordered_set<propnet::InputSet> invalid_inputs_cache;
                std::shared_mutex invalid_inputs_cache_lock;
            };

            std::deque<History> all_histories {};
            const propnet::Propnet& propnet;
            const propnet::Role& sampler_role;
            std::vector<propnet::Role> player_roles;
            std::optional<propnet::Role> random_role;

            using AllHistories = decltype(all_histories);

            std::optional<propnet::State> sample_state_impl(AllHistories::iterator all_histories_it, AllHistories::iterator all_histories_end_it, propnet::State state);
            inline static bool is_invalid_state(AllHistories::iterator all_histories_it, const propnet::State& state);
            inline static void add_invalid_state(AllHistories::iterator all_histories_it, const propnet::State& state);
            inline static bool is_invalid_inputs(AllHistories::iterator all_histories_it, const propnet::InputSet& inputs);
            inline static void add_invalid_inputs(AllHistories::iterator all_histories_it, const propnet::InputSet& inputs);
    };
}
