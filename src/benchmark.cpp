#include "../player/include/search.hpp"
#include "setup.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

static constexpr auto SCISSOR_PAPER_ROCK_GAME{"scissor_paper_rock"};
static constexpr auto SCISSOR_PAPER_ROCK_SAVE_FREQUENCY{2000};
static constexpr auto SCISSOR_PAPER_ROCK_NUM_ITERATIONS{16000};

static constexpr auto BLINDTICTACTOE_GAME{"blindtictactoe"};
static constexpr auto BLINDTICTACTOE_SAVE_FREQUENCY{1000};
static constexpr auto BLINDTICTACTOE_NUM_ITERATIONS{std::numeric_limits<std::size_t>::max()};

class BenchmarkLogger
{
  public:
    BenchmarkLogger(std::string_view game, std::size_t save_frequency);

    void log(const std::vector<std::reference_wrapper<player::search::InformationSet>> &current_information_sets);

  private:
    static constexpr auto BENCHMARK_FOLDER{"benchmarks"};

    static constexpr auto ITERATIONS_KEY{"iterations"};
    static constexpr auto PROBABILITIES_KEY{"probabilities"};
    static constexpr auto TIME_KEY{"time"};

    void save() const;
    static std::chrono::microseconds get_time_us();

    std::string game;
    std::size_t save_frequency;
    std::size_t iteration_count;
    nlohmann::json json_log;
    std::chrono::microseconds cumulative_time_taken;
    std::chrono::microseconds last_log_time;
};

BenchmarkLogger::BenchmarkLogger(std::string_view game, std::size_t save_frequency)
    : game{game}, save_frequency{save_frequency}, iteration_count{0},
      json_log{{ITERATIONS_KEY, nlohmann::json::array()}}, cumulative_time_taken{0}, last_log_time{get_time_us()}
{
}

void BenchmarkLogger::log(
    const std::vector<std::reference_wrapper<player::search::InformationSet>> &current_information_sets)
{
    ++iteration_count;
    const auto time_since_last_log{get_time_us() - last_log_time};
    cumulative_time_taken += time_since_last_log;

    nlohmann::json current_log{{TIME_KEY, cumulative_time_taken.count()},
                               {PROBABILITIES_KEY, nlohmann::json::object()}};
    auto &current_probabilities_log{current_log[PROBABILITIES_KEY]};
    for (const auto &current_information_set : current_information_sets)
    {
        const auto [policy, _]{current_information_set.get().normalise()};
        for (const auto &[legal_input, probability] : policy)
        {
            current_probabilities_log[std::to_string(legal_input)] = probability;
        }
    }

    json_log[ITERATIONS_KEY].push_back(std::move(current_log));

    if (iteration_count % save_frequency == 0)
    {
        save();
    }

    const auto end_log_time{get_time_us()};
    last_log_time = end_log_time;
}

void BenchmarkLogger::save() const
{
    std::stringstream path_stream{};
    path_stream << BENCHMARK_FOLDER << '/' << game << '/';
    path_stream << std::setfill('0') << std::setw(10) << iteration_count;
    path_stream << ".json";
    const auto path{path_stream.str()};

    std::cout << "Outputting to " << path << '\n';

    std::ofstream output_file{path};
    output_file << json_log;
    output_file.close();
}

std::chrono::microseconds BenchmarkLogger::get_time_us()
{
    const auto now_time{std::chrono::system_clock::now()};
    const auto now_time_us = std::chrono::duration_cast<std::chrono::microseconds>(now_time.time_since_epoch());

    return now_time_us;
}

void benchmark(std::string_view game, std::size_t save_frequency, std::size_t num_iterations)
{
    const propnet::Propnet propnet{setup::load_propnet(game)};
    player::search::FullMCCFR mccfr{propnet};
    const auto initial_state{propnet.create_initial_state()};

    BenchmarkLogger logger{game, save_frequency};
    mccfr.search(initial_state, num_iterations,
                 [&logger](const auto &current_information_sets) { logger.log(current_information_sets); });
}

void benchmark_scissor_paper_rock()
{
    benchmark(SCISSOR_PAPER_ROCK_GAME, SCISSOR_PAPER_ROCK_SAVE_FREQUENCY, SCISSOR_PAPER_ROCK_NUM_ITERATIONS);
}

void benchmark_blindtictactoe()
{
    benchmark(BLINDTICTACTOE_GAME, BLINDTICTACTOE_SAVE_FREQUENCY, BLINDTICTACTOE_NUM_ITERATIONS);
}

int main(void)
{
    /*
    Manually change the game you want to benchmark here
    */
    benchmark_scissor_paper_rock();

    return 0;
}
