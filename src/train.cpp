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
static constexpr auto NUM_CONCURRENT_GAMES_COMMAND{"num_concurrent_games"};
static constexpr auto DEFAULT_NUM_CONCURRENT_GAMES{1};
static constexpr auto TIME_LIMIT_COMMAND{"time_limit"};
static constexpr auto GAME_COMMAND{"game"};
static constexpr auto READABLE_TIME_FORMAT{"%X %e %b %Y %Z"};
static constexpr auto COMPLETE_CFR_CACHE_SIZE{static_cast<std::size_t>(1e4)};
static constexpr auto TIME_LOG_FILE_NAME{"time-log.txt"};
static constexpr std::size_t MIN_GAMES_PER_MODEL_SAVE{10};

using CompleteCfrCache =
    player::misc::Cache<propnet::State,
                        std::shared_future<std::vector<std::pair<player::Policy, player::ExpectedValue>>>,
                        caches::LRUCachePolicy, COMPLETE_CFR_CACHE_SIZE>;

std::function<bool()> get_time_limit_function(std::optional<std::size_t> time_limit);
std::string to_readable_time(const std::chrono::time_point<std::chrono::system_clock> &time_point);
void train(std::size_t num_concurrent_games, const std::function<bool()> &time_limit_function, std::string_view game);
void play_concurrent_games(const propnet::Propnet &propnet, player::ReplayBuffer &replay_buffer,
                           std::vector<std::vector<player::Player<>>> &all_players,
                           std::vector<std::optional<agents::RandomAgent>> &all_random);
void play_game(const propnet::Propnet &propnet, player::ReplayBuffer &replay_buffer, std::mutex &replay_buffer_lock,
               CompleteCfrCache &complete_cfr_cache, std::vector<player::Player<>> &players,
               std::optional<agents::RandomAgent> &random);

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
    std::size_t num_concurrent_games;
    std::size_t time_limit;
    std::string game{};
    po::options_description options_description{"Allowed options"};
    options_description.add_options()(HELP_COMMAND, "produce help message")(
        NUM_CONCURRENT_GAMES_COMMAND,
        po::value<std::size_t>(&num_concurrent_games)->default_value(DEFAULT_NUM_CONCURRENT_GAMES),
        "set number of games to play concurrently for training")(
        TIME_LIMIT_COMMAND, po::value<std::size_t>(&time_limit), "maximum time allowed to train (in minutes)")(
        GAME_COMMAND, po::value<std::string>(&game)->required(), "game to train on");

    po::variables_map variables_map;
    po::store(po::parse_command_line(argc, argv, options_description), variables_map);
    try
    {
        po::notify(variables_map);
    }
    catch (const boost::program_options::required_option &error)
    {
        if (variables_map.contains(HELP_COMMAND))
        {
            std::cout << options_description;
            return 2;
        }
        else
        {
            std::cout << error.what();
            return 1;
        }
    }

    if (variables_map.contains(HELP_COMMAND))
    {
        std::cout << options_description;
        return 2;
    }

    std::cout << "Training with " << num_concurrent_games << " concurrent game(s)\n\n";

    const auto is_time_limit{variables_map.contains(TIME_LIMIT_COMMAND)};
    const auto time_limit_function{
        get_time_limit_function(is_time_limit ? std::optional<std::size_t>{time_limit} : std::nullopt)};

    // TODO: Setup
    if (num_concurrent_games != 1)
    {
        throw std::logic_error{"Not set up to work for more than 1 game currently"};
    }
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
        std::cout << "End time is " << to_readable_time(end_time) << '\n';

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
    std::cout << '\n';

    player::ReplayBuffer replay_buffer{};
    auto model{setup::load_model(propnet, game)};
    std::cout << '\n';

    const auto hardware_threads{std::thread::hardware_concurrency()};
    const auto threads_per_player{hardware_threads / (2 * propnet.num_player_roles())};
    std::cout << "Using " << threads_per_player << " threads per player\n\n";

    std::vector<std::vector<player::Player<>>> all_players{};
    std::vector<std::optional<agents::RandomAgent>> all_random{};
    for (std::size_t game_count{0}; game_count < num_concurrent_games; ++game_count)
    {
        std::vector<player::Player<>> players{};
        for (const auto &role : propnet.get_player_roles())
        {
            players.emplace_back(role, propnet, model, threads_per_player);
        }
        all_players.push_back(std::move(players));

        all_random.emplace_back(propnet.is_randomness() ? std::optional<agents::RandomAgent>{*propnet.get_random_role()}
                                                        : std::nullopt);
    }

    std::size_t game_number{0};
    std::size_t last_save_game_number{0};
    while (time_limit_function())
    {
        play_concurrent_games(propnet, replay_buffer, all_players, all_random);
        model.train(replay_buffer);

        game_number += num_concurrent_games;
        std::cout << game_number << " game(s) played\n";

        const auto games_since_last_save{game_number - last_save_game_number};
        if (games_since_last_save >= MIN_GAMES_PER_MODEL_SAVE)
        {
            std::cout << '\n';
            model.save(game_number);

            std::cout << "Saved on game number " << game_number << "\n\n";
            last_save_game_number = game_number;
        }
    }
}

void play_concurrent_games(const propnet::Propnet &propnet, player::ReplayBuffer &replay_buffer,
                           std::vector<std::vector<player::Player<>>> &all_players,
                           std::vector<std::optional<agents::RandomAgent>> &all_random)
{
    std::mutex replay_buffer_lock{};

    CompleteCfrCache complete_cfr_cache{};

    std::vector<std::thread> threads{};
    auto all_random_it{all_random.begin()};
    for (auto &players : all_players)
    {
        threads.emplace_back(
            [&propnet, &replay_buffer, &replay_buffer_lock, &complete_cfr_cache, &players, all_random_it]() {
                play_game(propnet, replay_buffer, replay_buffer_lock, complete_cfr_cache, players, *all_random_it);
            });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
}

void play_game(const propnet::Propnet &propnet, player::ReplayBuffer &replay_buffer, std::mutex &replay_buffer_lock,
               CompleteCfrCache &complete_cfr_cache, std::vector<player::Player<>> &players,
               std::optional<agents::RandomAgent> &random)
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

        std::vector<std::future<propnet::PropId>> input_futures{};
        for (auto &player : players)
        {
            input_futures.push_back(
                std::async(std::launch::async, [&player, &state]() { return player.get_legal_input(state); }));
        }

        propnet::InputSet inputs{};
        if (random.has_value())
        {
            const auto input{random->get_legal_input(state)};
            inputs.add(input);
        }

        for (auto &input_future : input_futures)
        {
            input_future.wait();
            const auto input{input_future.get()};
            inputs.add(input);
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
    }
}
