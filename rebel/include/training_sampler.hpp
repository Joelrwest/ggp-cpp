#pragma once

#include "../src/sampler.hpp"
#include "../src/misc.hpp"

#include <tuple>
#include <shared_mutex>
#include <list>

namespace rebel
{
    class TrainingSampler : public Sampler
    {
        public:
            TrainingSampler(const propnet::Role& sampler_role, const propnet::Propnet& propnet);

            void prepare_new_game();
            void add_history(const std::vector<bool>& observation, std::uint32_t prev_input);
            propnet::State sample_state();
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
            };

            std::list<History> all_histories {};
            const propnet::Propnet& propnet;
            const propnet::Role& sampler_role;
            std::vector<agents::RandomAgent> player_agents;
            std::optional<agents::RandomAgent> random_agent;

            using AllHistories = decltype(all_histories);

            std::optional<propnet::State> sample_state_impl(AllHistories::iterator all_histories_it, AllHistories::iterator all_histories_end_it, propnet::State state);
    };
}
