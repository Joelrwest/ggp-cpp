"""
File that takes in a benchmark file and outputs plots for scissor paper rock game.

All plots written using:
https://www.geeksforgeeks.org/how-to-plot-a-time-series-in-matplotlib/
"""

import matplotlib.pyplot as plt
import json

# Can change the input files, the time taken, and the colours used from these top constants

FILENAME = 'speed-0000044000.json'

SINGLE_COLOURS = (
    'orchid',
    'indigo',
)

GROUP_COLOURS = (
    SINGLE_COLOURS,
    (
        'salmon',
        'maroon',
    ),
    (
        'mediumseagreen',
        'darkgreen',
    ),
    (
        'cornflowerblue',
        'mediumblue',
    ),
)

X_AXIS_NAME_TIME = 'Time (h)'
X_AXIS_NAME_NUM_ITERATIONS = 'Number of Iterations'

FONT_SIZE = 12

# End of changable constants

plt.rcParams.update({'font.size': FONT_SIZE})

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
        MARK_1_1: '210',
        MARK_1_2: '80',
        MARK_1_3: '542',
        MARK_2_1: '150',
        MARK_2_2: '74',
        MARK_2_3: '195',
        MARK_3_1: '69',
        MARK_3_2: '211',
        MARK_3_3: '522',
    },
    PLAYER_O: {
        MARK_1_1: '44',
        MARK_1_2: '378',
        MARK_1_3: '259',
        MARK_2_1: '265',
        MARK_2_2: '92',
        MARK_2_3: '13',
        MARK_3_1: '148',
        MARK_3_2: '416',
        MARK_3_3: '86',
    },
}

PLAYERS = [
    player
    for player in MOVES_TO_INPUTS.keys()
]

def get_time_data_h(iterations):
    return [
        iteration[TIME_KEY] / (60 * 60 * 1e6)
        for iteration in iterations
    ]

def get_iteration_num_data(iterations):
    return [
        i for i in range(len(iterations))
    ]

def create_convergence_plot(iterations, x: list[float], player: str, move: str, colour):
    y = [
        iteration[PROBABILITIES_KEY][MOVES_TO_INPUTS[player][move]]
        for iteration in iterations
    ]

    plt.plot(x, y, color=colour)

    plt.ylabel('Probability')

def main() -> None:
    with open(FILENAME, 'r') as file:
        iterations = json.loads(file.read())[ITERATIONS_KEY]

    x_time = get_time_data_h(iterations)
    x_iteration_num = get_iteration_num_data(iterations)
    iterations = iterations[:len(x_time)]

    for player, colour in zip(PLAYERS, SINGLE_COLOURS):
        for move in ALL_MOVES:
            create_convergence_plot(iterations, x_time, player, move, colour)
            plt.xlabel(X_AXIS_NAME_TIME)
            plt.title(f"Convergence of {move.capitalize()} for {player} Over Time")
            plt.tight_layout()
            plt.savefig(f"{player}-{move.lower().replace(' ', '-')}-time-convergence-plot")
            plt.clf()

    for player, colour in zip(PLAYERS, SINGLE_COLOURS):
        for move in ALL_MOVES:
            create_convergence_plot(iterations, x_iteration_num, player, move, colour)
            plt.xlabel(X_AXIS_NAME_NUM_ITERATIONS)
            plt.title(f"Convergence of {move.capitalize()} for {player}")
            plt.tight_layout()
            plt.savefig(f"{player}-{move.lower().replace(' ', '-')}-iteration-num-convergence-plot")
            plt.clf()

    for idx, player in enumerate(PLAYERS):
        for title, similar_moves in SIMILAR_MOVES:
            group_colours = [
                colours[idx]
                for colours in GROUP_COLOURS
            ]

            for move, colours in zip(similar_moves, group_colours):
                create_convergence_plot(iterations, x_time, player, move, colours)
            plt.xlabel(X_AXIS_NAME_TIME)
            plt.title(f"Convergence of {title} for {player} Over Time")
            plt.tight_layout()
            plt.savefig(f"{player}-{title.lower().replace(' ', '-')}-time-convergence-plot")
            plt.clf()

            for move, colours in zip(similar_moves, group_colours):
                create_convergence_plot(iterations, x_iteration_num, player, move, colours)
            plt.xlabel(X_AXIS_NAME_NUM_ITERATIONS)
            plt.title(f"Convergence of {title} for {player}")
            plt.tight_layout()
            plt.savefig(f"{player}-{title.lower().replace(' ', '-')}-iteration-num-convergence-plot")
            plt.clf()

    for title, similar_moves in SIMILAR_MOVES:
        for idx, player in enumerate(PLAYERS):
            group_colours = [
                colours[idx]
                for colours in GROUP_COLOURS
            ]
            for move, colours in zip(similar_moves, group_colours):
                create_convergence_plot(iterations, x_time, player, move, colours)
        plt.xlabel(X_AXIS_NAME_TIME)
        plt.title(f"Convergence of {title} Over Time")
        plt.tight_layout()
        plt.savefig(f"{title.lower().replace(' ', '-')}-time-convergence-plot")
        plt.clf()

        for idx, player in enumerate(PLAYERS):
            group_colours = [
                colours[idx]
                for colours in GROUP_COLOURS
            ]
            for move, colours in zip(similar_moves, group_colours):
                create_convergence_plot(iterations, x_iteration_num, player, move, colours)
        plt.xlabel(X_AXIS_NAME_NUM_ITERATIONS)
        plt.title(f"Convergence of {title}")
        plt.tight_layout()
        plt.savefig(f"{title.lower().replace(' ', '-')}-iteration-num-convergence-plot")
        plt.clf()

if __name__ == '__main__':
    main()
