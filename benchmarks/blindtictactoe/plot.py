"""
File that takes in a benchmark file and outputs plots for scissor paper rock game.

All plots written using:
https://www.geeksforgeeks.org/how-to-plot-a-time-series-in-matplotlib/
"""

import matplotlib.pyplot as plt
import json

BENCHMARK_FILENAME = '0000001200.json'

PLAYER_X = 'x'
PLAYER_O = 'o'

MARK_1_1 = 'mark 1 1'
MARK_1_2 = 'mark 1 2'
MARK_1_3 = 'mark 1 3'
MARK_2_1 = 'mark 2 1'
MARK_2_2 = 'mark 2 2'
MARK_2_3 = 'mark 2 3'
MARK_3_1 = 'mark 3 1'
MARK_3_2 = 'mark 3 2'
MARK_3_3 = 'mark 3 3'

ALL_MOVES = (
    MARK_1_1,
    MARK_1_2,
    MARK_1_3,
    MARK_2_1,
    MARK_2_2,
    MARK_2_3,
    MARK_3_1,
    MARK_3_2,
    MARK_3_3,
)

ITERATIONS_KEY = 'iterations'
PROBABILITIES_KEY = 'probabilities'
TIME_KEY = 'time'

MOVES_TO_INPUTS = {
    PLAYER_X: {
        MARK_1_1: '153',
        MARK_1_2: '82',
        MARK_1_3: '528',
        MARK_2_1: '123',
        MARK_2_2: '324',
        MARK_2_3: '416',
        MARK_3_1: '73',
        MARK_3_2: '173',
        MARK_3_3: '471',
    },
    PLAYER_O: {
        MARK_1_1: '57',
        MARK_1_2: '268',
        MARK_1_3: '500',
        MARK_2_1: '41',
        MARK_2_2: '62',
        MARK_2_3: '137',
        MARK_3_1: '192',
        MARK_3_2: '531',
        MARK_3_3: '207',
    },
}

X_AXIS_NAME = 'Time (m)'

def get_time_data_m(data):
    return [
        iteration[TIME_KEY] / 60e6
        for iteration in data[ITERATIONS_KEY]
    ]

def create_convergence_graph(data, x: list[float], move: str):
    iterations = data[ITERATIONS_KEY]

    y_x = [
        iteration[PROBABILITIES_KEY][MOVES_TO_INPUTS[PLAYER_X][move]]
        for iteration in iterations
    ]

    y_o = [
        iteration[PROBABILITIES_KEY][MOVES_TO_INPUTS[PLAYER_O][move]]
        for iteration in data[ITERATIONS_KEY]
    ]

    plt.plot(x, y_x, color='red')
    plt.plot(x, y_o, color='green')

    plt.xticks(rotation=30, ha='right')

    plt.xlabel(X_AXIS_NAME)
    plt.ylabel(f"Convergence of {move.capitalize()} Over Time")

    plt.savefig(f"scissor-paper-rock-{move.lower().replace(' ', '-')}-convergence-plot")

def main() -> None:
    with open(BENCHMARK_FILENAME, 'r') as file:
        data = json.loads(file.read())

    x = get_time_data_m(data)

    for move in ALL_MOVES:
        create_convergence_graph(data, x, move)
        plt.clf()

if __name__ == '__main__':
    main()
