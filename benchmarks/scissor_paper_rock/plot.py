"""
File that takes in a benchmark file and outputs plots for scissor paper rock game.

All plots written using:
https://www.geeksforgeeks.org/how-to-plot-a-time-series-in-matplotlib/
"""

import matplotlib.pyplot as plt
import json

BENCHMARK_FILENAME = '0000002000.json'

PLAYER_1 = 'player1'
PLAYER_2 = 'player2'

ROCK = 'rock'
PAPER = 'paper'
SCISSORS = 'scissors'

ITERATIONS_KEY = 'iterations'
PROBABILITIES_KEY = 'probabilities'
TIME_KEY = 'time'

MOVES_TO_INPUTS = {
    PLAYER_1: {
        ROCK: '48',
        PAPER: '11',
        SCISSORS: '1',
    },
    PLAYER_2: {
        ROCK: '69',
        PAPER: '57',
        SCISSORS: '44',
    },
}

IDEAL_PROBABILITIES = {
    ROCK: 0.4,
    PAPER: 0.4,
    SCISSORS: 0.2,
}

X_AXIS_NAME = 'Time (ms)'

def get_time_data_ms(data):
    return [
        iteration[TIME_KEY] / 1e3
        for iteration in data[ITERATIONS_KEY]
    ]

def calculate_non_adapting_ev(probabilities: dict[str, float], player: str):
    """
    Hardcoded values from the gdl rules file.
    """
    ev_rock =  50 * IDEAL_PROBABILITIES[ROCK] + 25 * IDEAL_PROBABILITIES[PAPER] + 100 * IDEAL_PROBABILITIES[SCISSORS]
    ev_paper = 75 * IDEAL_PROBABILITIES[ROCK] + 50 * IDEAL_PROBABILITIES[PAPER]
    ev_scissors =  100 * IDEAL_PROBABILITIES[PAPER] + 50 * IDEAL_PROBABILITIES[SCISSORS]

    return (
        probabilities[MOVES_TO_INPUTS[player][ROCK]] * ev_rock +
        probabilities[MOVES_TO_INPUTS[player][PAPER]] * ev_paper +
        probabilities[MOVES_TO_INPUTS[player][SCISSORS]] * ev_scissors
    )

def calculate_adapting_ev(probabilities: dict[str, float], player: str):
    opponent_ev_rock =  50 * probabilities[MOVES_TO_INPUTS[player][ROCK]] + 25 * probabilities[MOVES_TO_INPUTS[player][PAPER]] + 100 * probabilities[MOVES_TO_INPUTS[player][SCISSORS]]
    opponent_ev_paper = 75 * probabilities[MOVES_TO_INPUTS[player][ROCK]] + 50 * probabilities[MOVES_TO_INPUTS[player][PAPER]]
    opponent_ev_scissors =  100 * probabilities[MOVES_TO_INPUTS[player][PAPER]] + 50 * probabilities[MOVES_TO_INPUTS[player][SCISSORS]]

    opponent_ev = max(opponent_ev_rock, opponent_ev_paper, opponent_ev_scissors)
    return 100 - opponent_ev # Can be done this way since it's a 0 sum game

def create_ev_data(data, x: list[float], calculate_ev_function) -> None:
    y_1 = [
        calculate_ev_function(iteration[PROBABILITIES_KEY], PLAYER_1)
        for iteration in data[ITERATIONS_KEY]
    ]

    y_2 = [
        calculate_ev_function(iteration[PROBABILITIES_KEY], PLAYER_2)
        for iteration in data[ITERATIONS_KEY]
    ]

    plt.plot(x, y_1, color='red')
    plt.plot(x, y_2, color='green')

    plt.xticks(rotation=30, ha='right')

    plt.xlabel(X_AXIS_NAME)

def create_non_adapting_ev_plot(data, x: list[float]) -> None:
    create_ev_data(data, x, calculate_non_adapting_ev)

    plt.ylabel('Expected Value Against Non-Adapting Optimal Opponent')

    plt.savefig('non-adapting-ev-plot')

def create_adapting_ev_plot(data, x: list[float]) -> None:
    create_ev_data(data, x, calculate_adapting_ev)

    y_3 = [
        50
        for _ in data[ITERATIONS_KEY]
    ]

    plt.plot(x, y_3, color='blue', linestyle='dotted')

    plt.ylabel('Expected Value Against Adapting Optimal Opponent')

    plt.savefig('adapting-ev-plot')

def create_convergence_graph(data, x: list[float], move: str):
    iterations = data[ITERATIONS_KEY]

    y_1 = [
        iteration[PROBABILITIES_KEY][MOVES_TO_INPUTS[PLAYER_1][move]]
        for iteration in iterations
    ]

    y_2 = [
        iteration[PROBABILITIES_KEY][MOVES_TO_INPUTS[PLAYER_2][move]]
        for iteration in data[ITERATIONS_KEY]
    ]

    y_3 = [
        IDEAL_PROBABILITIES[move]
        for _ in data[ITERATIONS_KEY]
    ]

    plt.plot(x, y_1, color='red')
    plt.plot(x, y_2, color='green')
    plt.plot(x, y_3, color='blue', linestyle='dotted')

    plt.xticks(rotation=30, ha='right')

    plt.xlabel(X_AXIS_NAME)
    plt.ylabel(f"Convergence of {move.capitalize().replace(' ', '-')} Over Time")

    plt.savefig(f"{move.lower()}-convergence-plot")

def main() -> None:
    with open(BENCHMARK_FILENAME, 'r') as file:
        data = json.loads(file.read())

    x = get_time_data_ms(data)

    create_non_adapting_ev_plot(data, x)
    plt.clf()
    create_adapting_ev_plot(data, x)
    plt.clf()

    for move in (ROCK, PAPER, SCISSORS):
        create_convergence_graph(data, x, move)
        plt.clf()

if __name__ == '__main__':
    main()
