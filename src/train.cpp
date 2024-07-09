#include "../player/include/misc.hpp"
#include "../player/include/model.hpp"
#include "../player/include/search.hpp"
#include "setup.hpp"

#include <boost/program_options.hpp>

#include <chrono>
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
static constexpr auto COMPLETE_CFR_CACHE_SIZE{10000};

using CompleteCfrCache =
    player::misc::Cache<propnet::State,
                        std::shared_future<std::vector<std::pair<player::Policy, player::ExpectedValue>>>,
                        caches::LRUCachePolicy, COMPLETE_CFR_CACHE_SIZE>;

std::function<bool()> get_time_limit_function(std::optional<std::size_t> time_limit);
std::string to_readable_time(const std::chrono::time_point<std::chrono::system_clock> &time_point);
void train(std::size_t num_concurrent_games, const std::function<bool()> &time_limit_function, std::string_view game);
void train_batch(std::size_t num_concurrent_games, const propnet::Propnet &propnet,
                 player::ReplayBuffer &replay_buffer);
void play_game(const propnet::Propnet &propnet, player::ReplayBuffer &replay_buffer, std::mutex &replay_buffer_lock,
               CompleteCfrCache &complete_cfr_cache);

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
    while (time_limit_function())
    {
        train_batch(num_concurrent_games, propnet, replay_buffer);
    }
}

void train_batch(std::size_t num_concurrent_games, const propnet::Propnet &propnet, player::ReplayBuffer &replay_buffer)
{
    auto remaining_games{BATCH_SIZE};
    std::mutex remaining_games_lock{};

    std::mutex replay_buffer_lock{};

    CompleteCfrCache complete_cfr_cache{};

    std::vector<std::thread> threads{};
    for (auto thread_count{0u}; thread_count < num_concurrent_games; ++thread_count)
    {
        threads.emplace_back([&propnet, &remaining_games, &remaining_games_lock, &replay_buffer, &replay_buffer_lock,
                              &complete_cfr_cache]() {
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

                play_game(propnet, replay_buffer, replay_buffer_lock, complete_cfr_cache);
            }
        });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
}

void play_game(const propnet::Propnet &propnet, player::ReplayBuffer &replay_buffer, std::mutex &replay_buffer_lock,
               CompleteCfrCache &complete_cfr_cache)
{
    auto state{propnet.create_initial_state()};
    while (!propnet.is_game_over(state))
    {
        if (!complete_cfr_cache.Cached(state))
        {
            complete_cfr_cache.Put(state, std::async(std::launch::async, [&propnet, &state]() {
                                       player::search::ExternalSamplingMCCFR mccfr{propnet};
                                       // return std::vector<std::unordered_map<propnet::PropId, double>>
                                       // {mccfr.search(state).first}; // TODO
                                       return std::vector<std::pair<player::Policy, player::ExpectedValue>>{};
                                   }));
        }

        const auto &future{*complete_cfr_cache.Get(state).get()};
        future.wait();

        const auto &cfr{future.get()};
        (void)cfr;
        (void)replay_buffer;

        {
            std::lock_guard replay_buffer_guard{replay_buffer_lock};
            // TODO: replay_buffer.add(state, cfr);
        }

        // for each agent do
        // Perform CFR on states sampled with this agent’s history
        // Make moves proportionally to new policy probabilities
        // end for
        // Sample and train neural network on 20 mini-batches from replay buffer
    }
}
