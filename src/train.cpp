#include "setup.hpp"
#include "../rebel/include/model.hpp"

#include <boost/program_options.hpp>

#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <optional>

namespace po = boost::program_options;

static constexpr auto HELP_COMMAND {"help"};
static constexpr auto NUM_THREADS_COMMAND {"total_num_threads"};
static constexpr auto TIME_LIMIT_COMMAND {"time_limit"};
static constexpr auto GAME_COMMAND {"game"};
static constexpr auto READABLE_TIME_FORMAT {"%X %e %b %Y %Z"};
static constexpr auto BATCH_SIZE {20};

std::function<bool()> get_time_limit_function(std::optional<unsigned int> time_limit);
std::string to_readable_time(const std::chrono::time_point<std::chrono::system_clock>& time_point);
void train(unsigned int total_num_threads, const std::function<bool()>& time_limit_function, std::string_view game);

class TimeLimit
{
    public:
        TimeLimit(const std::chrono::time_point<std::chrono::system_clock>& end_time) : end_time {end_time} {}

        bool operator()() const
        {
            return std::chrono::system_clock::now() < end_time;
        }
    private:
        std::chrono::time_point<std::chrono::system_clock> end_time;
};

int main(int argc, char** argv)
{
    const auto hardware_threads {std::thread::hardware_concurrency()};

    unsigned int total_num_threads;
    unsigned int time_limit;
    std::string game {};
    po::options_description options_description {"Allowed options"};
    options_description.add_options()
        (HELP_COMMAND, "produce help message")
        (NUM_THREADS_COMMAND, po::value<unsigned int>(&total_num_threads)->default_value(hardware_threads), "set number of threads for training")
        (TIME_LIMIT_COMMAND, po::value<unsigned int>(&time_limit), "maximum time allowed to train (in minutes)")
        (GAME_COMMAND, po::value<std::string>(&game)->required(), "game to train on")
    ;

    po::variables_map variables_map;
    po::store(po::parse_command_line(argc, argv, options_description), variables_map);
    try
    {
        po::notify(variables_map);
    }
    catch (const boost::program_options::required_option& error)
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

    const auto is_time_limit {variables_map.contains(TIME_LIMIT_COMMAND)};
    const auto time_limit_function {get_time_limit_function(
        is_time_limit ? std::optional<unsigned int> {time_limit} : std::nullopt
    )};

    train(
        total_num_threads,
        time_limit_function,
        game
    );

    std::cout << "Training complete!\n";

    return 0;
}

std::function<bool()> get_time_limit_function(std::optional<unsigned int> time_limit)
{
    const auto start_time {std::chrono::system_clock::now()};
    std::cout << "Start time is " << to_readable_time(start_time) << '\n';

    if (time_limit.has_value())
    {
        const std::chrono::minutes time_limit_duration {*time_limit};
        const auto end_time {start_time + time_limit_duration};
        std::cout << "End time is " << to_readable_time(end_time) << "\n\n";

        return TimeLimit {end_time};
    }
    else
    {
        std::cout << '\n';
        return []() { return true; };
    }
}

std::string to_readable_time(const std::chrono::time_point<std::chrono::system_clock>& time_point)
{
    const auto time_point_t {std::chrono::system_clock::to_time_t(time_point)};
    std::stringstream readable_time {};
    readable_time << std::put_time(std::localtime(&time_point_t), READABLE_TIME_FORMAT);

    return readable_time.str();
}

void train(unsigned int total_num_threads, const std::function<bool()>& time_limit_function, std::string_view game)
{
    (void)total_num_threads;
    (void)time_limit_function;
    const auto propnet {setup::load_propnet(game)};

    const auto num_threads_per_game {2}; // TODO: Profile this, see if more would be sufficient
    const auto num_concurrent_games {total_num_threads / num_threads_per_game + 1}; // Cheeky + 1
    std::vector<std::thread> threads {};
    auto model {rebel::Model {propnet, game}};
    for (auto thread_count {0u}; thread_count < num_concurrent_games; ++thread_count)
    {
        threads.emplace_back([&propnet, &time_limit_function, &model]() {
            // train(propnet, time_limit_function);
        });
    }

    std::for_each(
        threads.begin(),
        threads.end(),
        [](auto& thread) { thread.join(); }
    );
    (void)propnet;
    (void)time_limit_function;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Training!\n";

    std::thread actual_state_cfr_thread {[]() {
        return;
    }};

    actual_state_cfr_thread.join();

    // Reinitialise game state
    // while Game not finished do
        // Perform CFR on current game state
        // Add triple of (state, π, q) to the replay buffer
        // ▷ π and q represent the policies and values for all agents
        // for each agent do
            // Perform CFR on states sampled with this agent’s history
            // Make moves proportionally to new policy probabilities
        // end for
        // Sample and train neural network on 20 mini-batches from replay buffer
    // end while
}
