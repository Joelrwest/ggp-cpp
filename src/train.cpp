#include "../player/include/misc.hpp"
#include "../player/include/model.hpp"
#include "../player/include/player.hpp"
#include "../player/include/search.hpp"
#include "setup.hpp"

#include <boost/program_options.hpp>

#include <chrono>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <thread>

namespace po = boost::program_options;

static constexpr auto HELP_COMMAND{"help"};
static constexpr auto NUM_THREADS_COMMAND{"total_num_threads"};
static constexpr auto TIME_LIMIT_COMMAND{"time_limit"};
static constexpr auto GAME_COMMAND{"game"};
static constexpr auto READABLE_TIME_FORMAT{"%X %e %b %Y %Z"};
static constexpr auto BATCH_SIZE{20};
static constexpr auto COMPLETE_CFR_CACHE_SIZE{static_cast<std::size_t>(10e4)};
static constexpr auto TIME_LOG_FILE_NAME{"time-log.txt"};

using CompleteCfrCache =
    player::misc::Cache<propnet::State,
                        std::shared_future<std::vector<std::pair<player::Policy, player::ExpectedValue>>>,
                        caches::LRUCachePolicy, COMPLETE_CFR_CACHE_SIZE>;

std::function<bool()> get_time_limit_function(std::optional<std::size_t> time_limit);
std::string to_readable_time(const std::chrono::time_point<std::chrono::system_clock> &time_point);
void train(std::size_t num_concurrent_games, const std::function<bool()> &time_limit_function, std::string_view game);
void train_batch(const propnet::Propnet &propnet, player::ReplayBuffer &replay_buffer,
                 std::vector<std::vector<player::Player<>>> &all_players);
void play_game(const propnet::Propnet &propnet, player::ReplayBuffer &replay_buffer, std::mutex &replay_buffer_lock,
               CompleteCfrCache &complete_cfr_cache, std::vector<player::Player<>> &players);
std::filesystem::path get_time_log_file_path(std::string_view game);
std::chrono::milliseconds get_time_ms();

// const auto now_time_ms{get_time_ms()};
// const auto duration_ms{now_time_ms - start_time_ms};
// const auto num_ms{duration_ms.count()};
// const auto num_s{num_ms / 10e3};

// time_log_file << "Game number " << game_number << " took " << num_s << " seconds to reach\n";

class TimeLimit
{
  public:
    TimeLimit(const std::chrono::time_point<std::chrono::system_clock> &end_time) : end_time{end_time}
    {
    }

    bool operator()() const
    {
        return std::chrono::system_clock::now() < end_time;
    }

  private:
    std::chrono::time_point<std::chrono::system_clock> end_time;
};

int main(int argc, char **argv)
{
    const auto hardware_threads{std::thread::hardware_concurrency()};

    std::size_t total_num_threads;
    std::size_t time_limit;
    std::string game{};
    po::options_description options_description{"Allowed options"};
    options_description.add_options()(HELP_COMMAND, "produce help message")(
        NUM_THREADS_COMMAND, po::value<std::size_t>(&total_num_threads)->default_value(hardware_threads),
        "set number of threads for training")(TIME_LIMIT_COMMAND, po::value<std::size_t>(&time_limit),
                                              "maximum time allowed to train (in minutes)")(
        GAME_COMMAND, po::value<std::string>(&game)->required(), "game to train on");

    po::variables_map variables_map;
    po::store(po::parse_command_line(argc, argv, options_description), variables_map);
    try
    {
        po::notify(variables_map);
    }
    catch (const boost::program_options::required_option &error)
    {
        std::cout << error.what();
        return 1;
    }

    if (variables_map.contains(HELP_COMMAND))
    {
        std::cout << options_description << '\n';
        return 2;
    }

    if (total_num_threads < 2)
    {
        std::cout << "Code written under the assumption that there's at least 2 threads\n";
        return 3;
    }

    std::cout << "Training using " << total_num_threads << " threads\n";

    const auto is_time_limit{variables_map.contains(TIME_LIMIT_COMMAND)};
    const auto time_limit_function{
        get_time_limit_function(is_time_limit ? std::optional<std::size_t>{time_limit} : std::nullopt)};

    const auto num_concurrent_games{total_num_threads + 1}; // Cheeky + 1
    train(num_concurrent_games, time_limit_function, game);

    std::cout << "Training complete!\n";

    return 0;
}

std::function<bool()> get_time_limit_function(std::optional<std::size_t> time_limit)
{
    const auto start_time{std::chrono::system_clock::now()};
    std::cout << "Start time is " << to_readable_time(start_time) << '\n';

    if (time_limit.has_value())
    {
        const std::chrono::minutes time_limit_duration{*time_limit};
        const auto end_time{start_time + time_limit_duration};
        std::cout << "End time is " << to_readable_time(end_time) << "\n\n";

        return TimeLimit{end_time};
    }
    else
    {
        std::cout << '\n';
        return []() { return true; };
    }
}

std::string to_readable_time(const std::chrono::time_point<std::chrono::system_clock> &time_point)
{
    const auto time_point_t{std::chrono::system_clock::to_time_t(time_point)};
    std::stringstream readable_time{};
    readable_time << std::put_time(std::localtime(&time_point_t), READABLE_TIME_FORMAT);

    return readable_time.str();
}

void train(std::size_t num_concurrent_games, const std::function<bool()> &time_limit_function, std::string_view game)
{
    const auto propnet{setup::load_propnet(game)};
    player::ReplayBuffer replay_buffer{};
    auto model{setup::load_model(propnet, game)};

    std::vector<std::vector<player::Player<>>> all_players{};
    for (std::size_t game_count{0}; game_count < num_concurrent_games; ++game_count)
    {
        std::vector<player::Player<>> players{};
        for (const auto &role : propnet.get_player_roles())
        {
            players.emplace_back(role, propnet, model, 3); // TODO: Idk why 3
        }
        all_players.push_back(std::move(players));
    }

    // TODO: Setup
    if (num_concurrent_games != 1)
    {
        throw std::logic_error{"Not set up to work for more than 1 game currently"};
    }

    while (time_limit_function())
    {
        train_batch(propnet, replay_buffer, all_players);
    }
}

void train_batch(const propnet::Propnet &propnet, player::ReplayBuffer &replay_buffer,
                 std::vector<std::vector<player::Player<>>> &all_players)
{
    auto remaining_games{BATCH_SIZE};
    std::mutex remaining_games_lock{};

    std::mutex replay_buffer_lock{};

    CompleteCfrCache complete_cfr_cache{};

    std::vector<std::thread> threads{};

    for (auto &players : all_players)
    {
        threads.emplace_back([&propnet, &remaining_games, &remaining_games_lock, &replay_buffer, &replay_buffer_lock,
                              &complete_cfr_cache, &players]() { // TODO
            while (true)
            {
                {
                    std::lock_guard remaining_games_guard{remaining_games_lock};
                    if (remaining_games == 0)
                    {
                        return;
                    }

                    --remaining_games;
                }

                play_game(propnet, replay_buffer, replay_buffer_lock, complete_cfr_cache, players);
            }
        });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
}

void play_game(const propnet::Propnet &propnet, player::ReplayBuffer &replay_buffer, std::mutex &replay_buffer_lock,
               CompleteCfrCache &complete_cfr_cache, std::vector<player::Player<>> &players)
{
    auto state{propnet.create_initial_state()};
    for (auto &player : players)
    {
        player.prepare_new_game();
        player.take_observations(state);
    }

    while (!propnet.is_game_over(state))
    {
        if (!complete_cfr_cache.Cached(state))
        {
            complete_cfr_cache.Put(state, std::async(std::launch::async, [&propnet, state]() {
                                       player::search::FullMCCFR mccfr{propnet};
                                       return mccfr.search(state);
                                   }));
        }

        const auto &cfr_future{*complete_cfr_cache.Get(state).get()};

        propnet::InputSet inputs{};
        for (auto &player : players)
        {
            const auto input{player.get_legal_input(state)};
            inputs.add(input);
            // Perform CFR on states sampled with this agent’s history
            // Make moves proportionally to new policy probabilities
        }

        propnet.take_sees_inputs(state, inputs);
        for (auto &player : players)
        {
            player.take_observations(state);
        }
        propnet.take_non_sees_inputs(state, inputs);

        cfr_future.wait();
        const auto &cfr_results{cfr_future.get()};

        std::vector<player::Policy> policies{};
        std::vector<player::ExpectedValue> values{};
        for (const auto &cfr_result : cfr_results)
        {
            policies.push_back(cfr_result.first);
            values.push_back(cfr_result.second);
        }

        {
            std::lock_guard replay_buffer_guard{replay_buffer_lock};
            replay_buffer.add(state, std::move(policies), std::move(values));
        }

        // TODO: This last works whilst it's single threaded, but not when it's multi
        std::vector<player::ReplayBuffer::Item> replay_buffer_sample{};
        {
            std::lock_guard replay_buffer_guard{replay_buffer_lock};
            replay_buffer_sample = replay_buffer.sample(BATCH_SIZE);
        }

        // Sample and train neural network on 20 mini-batches from replay buffer
    }
}

std::filesystem::path get_time_log_file_path(std::string_view game)
{
    auto path{player::Model::get_models_path(game)};
    path.append(TIME_LOG_FILE_NAME);

    return path;
}

std::chrono::milliseconds get_time_ms()
{
    const auto now_time{std::chrono::system_clock::now()};
    const auto now_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_time.time_since_epoch());

    return now_time_ms;
}
