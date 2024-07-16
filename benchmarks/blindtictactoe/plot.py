"""
File that takes in a benchmark file and outputs plots for scissor paper rock game.

All plots written using:
https://www.geeksforgeeks.org/how-to-plot-a-time-series-in-matplotlib/
"""

import matplotlib.pyplot as plt
import json

# Can change the input files, the time taken, and the colours used from these top constants

FILENAME = '0000001200.json'

COLOURS = (
    'indigo',
    'mediumorchid',
)

X_AXIS_NAME = 'Time (h)'

X_TICKS_ROTATION = 30

# End of changable constants

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

def get_time_data_h(data):
    return [
        iteration[TIME_KEY] / (60 * 60 * 1e6)
        for iteration in data[ITERATIONS_KEY]
    ]

def create_convergence_plot(data, x: list[float], move: str):
    num_points = len(x)
    iterations = data[ITERATIONS_KEY][:num_points]

    y_1 = [
        iteration[PROBABILITIES_KEY][MOVES_TO_INPUTS[PLAYER_X][move]]
        for iteration in iterations
    ]

    y_2 = [
        iteration[PROBABILITIES_KEY][MOVES_TO_INPUTS[PLAYER_O][move]]
        for iteration in iterations
    ]

    plt.plot(x, y_1, color=COLOURS[0])
    plt.plot(x, y_2, color=COLOURS[1])

    plt.xticks(rotation=X_TICKS_ROTATION, ha='right')

    plt.xlabel(X_AXIS_NAME)
    plt.ylabel('Probability of Playing')
    plt.title(f"Convergence of {move.capitalize()} Over Time")

    plt.tight_layout()

def main() -> None:
    with open(FILENAME, 'r') as file:
        data = json.loads(file.read())

    x = get_time_data_h(data)

    for move in ALL_MOVES:
        create_convergence_plot(data, x, move)
        plt.savefig(f"{move.lower().replace(' ', '-')}-convergence-plot")
        plt.clf()

if __name__ == '__main__':
    main()
