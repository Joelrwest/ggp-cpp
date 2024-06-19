#include "setup.hpp"
#include "../rebel/include/model.hpp"

#include <boost/program_options.hpp>

#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

namespace po = boost::program_options;

static constexpr auto HELP_COMMAND {"help"};
static constexpr auto NUM_THREADS_COMMAND {"num_threads"};
static constexpr auto TIME_LIMIT_COMMAND {"time_limit"};
static constexpr auto GAME_COMMAND {"game"};
static constexpr auto READABLE_TIME_FORMAT {"%X %e %b %Y %Z"};
static constexpr auto BATCH_SIZE {20};

std::string to_readable_time(const std::chrono::time_point<std::chrono::system_clock>& time_point);
void train(unsigned int num_threads, bool is_time_limit, unsigned int time_limit, std::string_view game);

int main(int argc, char** argv)
{
    const auto hardware_threads {std::thread::hardware_concurrency()};

    unsigned int num_threads;
    unsigned int time_limit;
    std::string game {};
    po::options_description options_description {"Allowed options"};
    options_description.add_options()
        (HELP_COMMAND, "produce help message")
        (NUM_THREADS_COMMAND, po::value<unsigned int>(&num_threads)->default_value(hardware_threads), "set number of threads for training")
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

    if (num_threads < 2)
    {
        std::cout << "Code written under the assumption that there's at least 2 threads\n";
        return 3;
    }

    std::cout << "Training using " << num_threads << " threads\n";

    const auto is_time_limit {variables_map.contains(TIME_LIMIT_COMMAND)};

    train(num_threads, is_time_limit, time_limit, game);

    std::cout << "Training complete!\n";

    return 0;
}

std::string to_readable_time(const std::chrono::time_point<std::chrono::system_clock>& time_point)
{
    const auto time_point_t {std::chrono::system_clock::to_time_t(time_point)};
    std::stringstream readable_time {};
    readable_time << std::put_time(std::localtime(&time_point_t), READABLE_TIME_FORMAT);

    return readable_time.str();
}

void train(unsigned int num_threads, bool is_time_limit, unsigned int time_limit, std::string_view game)
{
    (void)num_threads;
    const auto propnet {setup::load_propnet(game)};

    const auto start_time {std::chrono::system_clock::now()};
    std::cout << "Start time is " << to_readable_time(start_time) << '\n';

    const std::chrono::minutes time_limit_duration {time_limit};
    const auto end_time {start_time + time_limit_duration};
    if (is_time_limit)
    {
        std::cout << "End time is " << to_readable_time(end_time) << '\n';
    }
    std::cout << '\n';

    while (!is_time_limit || std::chrono::system_clock::now() < end_time)
    {
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
}
