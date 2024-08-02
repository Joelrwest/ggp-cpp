#include "../player/include/player.hpp"
#include "../propnet/include/propnet.hpp"
#include "../propnet/include/vector_state.hpp"
#include "setup.hpp"

#include <algorithm>
#include <iostream>

static constexpr auto NUM_GAMES{100};
static constexpr auto GAME{"blindtictactoe"};

void play_games(const propnet::Propnet &propnet, const propnet::Role &player_role, const propnet::Role &random_role,
                std::ofstream &file);

int main(void)
{
    const propnet::Propnet propnet{setup::load_propnet(GAME)};

    const auto &roles{propnet.get_player_roles()};

    std::thread thread_1{[&propnet, &roles]() {
        std::ofstream file{"x.txt"};
        play_games(propnet, roles[0], roles[1], file);
    }};

    std::thread thread_2{[&propnet, &roles]() {
        std::ofstream file{"o.txt"};
        play_games(propnet, roles[1], roles[0], file);
    }};

    thread_1.join();
    thread_2.join();

    return 0;
}

void play_games(const propnet::Propnet &propnet, const propnet::Role &player_role, const propnet::Role &random_role,
                std::ofstream &file)
{
    file << "Player " << player_role << ", random " << random_role << '\n';
    const std::chrono::seconds max_cfr_time{20};
    const std::chrono::seconds max_total_search_time{20};

    for (player::Depth depth_limit{2}; depth_limit < 4; ++depth_limit)
    {
        auto model{player::Model::load_game_number(propnet, GAME, 2000)};

        const auto player_options{player::Player<>::Options{}
                                      .add_cfr_time_limit(max_cfr_time)
                                      .add_max_sample_size(100)
                                      .add_total_time_limit(max_total_search_time)
                                      .add_thread_limit(16)
                                      .add_depth_limit(depth_limit)};

        std::vector<std::unique_ptr<agents::Agent>> agents{};
        agents.emplace_back(std::make_unique<player::Player<>>(player_role, propnet, model, player_options));
        agents.emplace_back(setup::agent_factory("random", random_role));

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
                propnet::InputSet inputs{};
                for (const auto &agent : agents)
                {
                    const auto input{agent->get_legal_input(state)};
                    inputs.add(input);
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
            file << "Total reward for '" << role << "' at depth " << depth_limit << ": " << total_reward << '\n';
        }
    }
}
