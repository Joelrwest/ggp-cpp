"""
File that takes in a benchmark file and outputs plots for scissor paper rock game.

All plots written using:
https://www.geeksforgeeks.org/how-to-plot-a-time-series-in-matplotlib/
"""

import matplotlib.pyplot as plt
import json

# Can change the input files, the time taken, and the colours used from these top constants

FILENAME = 'data.json'

SINGLE_COLOURS = (
    'indigo',
    'mediumorchid',
)

GROUP_COLOURS = (
    (
        'darkorange',
        'orange',
    ),
    (
        'maroon',
        'indianred',
    ),
    (
        'darkolivegreen',
        'olivedrab',
    ),
    (
        'mediumblue',
        'royalblue',
    ),
)

X_AXIS_NAME = 'Time (h)'

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

SIMILAR_MOVES = (
    (
        'Corner Moves',
        (
            MARK_1_1,
            MARK_1_3,
            MARK_3_1,
            MARK_3_3,
        )
    ),
    (
        'Middle-Side Moves',
        (
            MARK_1_2,
            MARK_2_1,
            MARK_2_3,
            MARK_3_2,
        ),
    ),
    (
        'Middle-Middle Move',
        (
            MARK_2_2,
        ),
    ),
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

def get_time_data_h(iterations):
    return [
        iteration[TIME_KEY] / (60 * 60 * 1e6)
        for iteration in iterations
    ]

def create_convergence_plot(iterations, x: list[float], move: str, colours: tuple[str]):
    y_1 = [
        iteration[PROBABILITIES_KEY][MOVES_TO_INPUTS[PLAYER_X][move]]
        for iteration in iterations
    ]

    y_2 = [
        iteration[PROBABILITIES_KEY][MOVES_TO_INPUTS[PLAYER_O][move]]
        for iteration in iterations
    ]

    plt.plot(x, y_1, color=colours[0])
    plt.plot(x, y_2, color=colours[1])

    plt.xlabel(X_AXIS_NAME)
    plt.ylabel('Probability')

def main() -> None:
    with open(FILENAME, 'r') as file:
        iterations = json.loads(file.read())[ITERATIONS_KEY]

    x = get_time_data_h(iterations)
    iterations = iterations[:len(x)]

    for move in ALL_MOVES:
        create_convergence_plot(iterations, x, move, SINGLE_COLOURS)
        plt.title(f"Convergence of {move.capitalize()} Over Time")
        plt.tight_layout()
        plt.savefig(f"{move.lower().replace(' ', '-')}-convergence-plot")
        plt.clf()

    for title, similar_moves in SIMILAR_MOVES:
        for move, colours in zip(similar_moves, GROUP_COLOURS):
            create_convergence_plot(iterations, x, move, colours)
        plt.title(f"Convergence of {title} Over Time")
        plt.tight_layout()
        plt.savefig(f"{title.lower().replace(' ', '-')}-convergence-plot")
        plt.clf()

if __name__ == '__main__':
    main()
