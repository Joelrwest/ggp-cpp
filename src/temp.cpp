#include "../player/include/player.hpp"
#include "../propnet/include/propnet.hpp"
#include "../propnet/include/vector_state.hpp"
#include "setup.hpp"

#include <algorithm>
#include <iostream>
#include <future>
#include <thread>

static constexpr auto NUM_GAMES{10};
static constexpr auto GAME{"blindtictactoe"};

int main(void)
{
    const propnet::Propnet propnet{setup::load_propnet(GAME)};

    const auto &roles{propnet.get_player_roles()};

    const std::chrono::seconds max_cfr_time{30};
    const std::chrono::seconds max_total_search_time{30};

    std::cout << roles[0] << ' ' << roles[1] << '\n';

    for (player::Depth depth_limit_1{1}; depth_limit_1 < 4; ++depth_limit_1)
    {
        for (player::Depth depth_limit_2{1}; depth_limit_2 <= depth_limit_1; ++depth_limit_2)
        {
            auto model{player::Model::load_game_number(propnet, GAME, 2000)};

            const auto player_options_1{player::Player<>::Options{}
                                        .add_cfr_time_limit(max_cfr_time)
                                        .add_max_sample_size(100)
                                        .add_total_time_limit(max_total_search_time)
                                        .add_thread_limit(12)
                                        .add_depth_limit(depth_limit_1)};

            const auto player_options_2{player::Player<>::Options{}
                                        .add_cfr_time_limit(max_cfr_time)
                                        .add_max_sample_size(100)
                                        .add_total_time_limit(max_total_search_time)
                                        .add_thread_limit(12)
                                        .add_depth_limit(depth_limit_2)};

            std::vector<std::unique_ptr<agents::Agent>> agents{};
            agents.emplace_back(std::make_unique<player::Player<>>(roles[0], propnet, model, player_options_1));
            agents.emplace_back(std::make_unique<player::Player<>>(roles[1], propnet, model, player_options_2));

            if (propnet.is_randomness())
            {
                agents.emplace_back(setup::agent_factory("random", *propnet.get_random_role()));
            }

            std::unordered_map<std::string, propnet::Reward> total_rewards{};
            for (const auto &agent : agents)
            {
                total_rewards.emplace(std::string{agent->get_role_name()}, 0);
            }

            for (auto game_count{0}; game_count < NUM_GAMES; ++game_count)
            {
                auto state{propnet.create_initial_state()};
                for (const auto &agent : agents)
                {
                    agent->prepare_new_game();
                    agent->take_observations(state);
                }

                while (!propnet.is_game_over(state))
                {
                    std::vector<std::future<propnet::PropId>> input_futures{};
                    for (const auto &agent : agents)
                    {
                        auto &player{*agent};
                        input_futures.push_back(std::async(
                            std::launch::async, [&player, &state]() { return player.get_legal_input(state); }));
                    }

                    propnet::InputSet inputs{};
                    for (auto &input_future : input_futures)
                    {
                        input_future.wait();
                        inputs.add(input_future.get());
                    }

                    propnet.take_sees_inputs(state, inputs);
                    for (const auto &agent : agents)
                    {
                        agent->take_observations(state);
                    }
                    propnet.take_non_sees_inputs(state, inputs);
                }

                for (const auto &agent : agents)
                {
                    const auto reward{agent->get_reward(state)};
                    const auto role{agent->get_role_name()};
                    total_rewards.at(std::string{role}) += reward;
                }
            }

            for (const auto &[role, total_reward] : total_rewards)
            {
                std::cout << "Total reward for '" << role << "' at depths " << depth_limit_1 << ", " << depth_limit_2 << ": " << total_reward << '\n';
            }
        }
    }

    return 0;
}
