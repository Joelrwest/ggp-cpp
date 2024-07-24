"""
File that takes in a benchmark file and outputs plots for scissor paper rock game.

All plots written using:
https://www.geeksforgeeks.org/how-to-plot-a-time-series-in-matplotlib/
"""

import matplotlib.pyplot as plt
import json

# Can change the input files, the time taken, and the colours used from these top constants

FILENAME = 'speed-0000010000.json'

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

X_AXIS_NAME = 'Time (h)'

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
        MARK_1_1: '445',
        MARK_1_2: '115',
        MARK_1_3: '304',
        MARK_2_1: '134',
        MARK_2_2: '506',
        MARK_2_3: '516',
        MARK_3_1: '281',
        MARK_3_2: '114',
        MARK_3_3: '52',
    },
    PLAYER_O: {
        MARK_1_1: '166',
        MARK_1_2: '596',
        MARK_1_3: '433',
        MARK_2_1: '472',
        MARK_2_2: '87',
        MARK_2_3: '171',
        MARK_3_1: '215',
        MARK_3_2: '373',
        MARK_3_3: '579',
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

def create_convergence_plot(iterations, x: list[float], player: str, move: str, colour):
    y = [
        iteration[PROBABILITIES_KEY][MOVES_TO_INPUTS[player][move]]
        for iteration in iterations
    ]

    plt.plot(x, y, color=colour)

    plt.xlabel(X_AXIS_NAME)
    plt.ylabel('Probability')

def main() -> None:
    with open(FILENAME, 'r') as file:
        iterations = json.loads(file.read())[ITERATIONS_KEY]

    x = get_time_data_h(iterations)
    iterations = iterations[:len(x)]

    for player, colour in zip(PLAYERS, SINGLE_COLOURS):
        for move in ALL_MOVES:
            create_convergence_plot(iterations, x, player, move, colour)
            plt.title(f"Convergence of {move.capitalize()} for {player} Over Time")
            plt.tight_layout()
            plt.savefig(f"{player}-{move.lower().replace(' ', '-')}-convergence-plot")
            plt.clf()

    for idx, player in enumerate(PLAYERS):
        for title, similar_moves in SIMILAR_MOVES:
            group_colours = (
                colours[idx]
                for colours in GROUP_COLOURS
            )

            for move, colours in zip(similar_moves, group_colours):
                create_convergence_plot(iterations, x, player, move, colours)
            plt.title(f"Convergence of {title} for {player} Over Time")
            plt.tight_layout()
            plt.savefig(f"{player}-{title.lower().replace(' ', '-')}-convergence-plot")
            plt.clf()

    for title, similar_moves in SIMILAR_MOVES:
        for idx, player in enumerate(PLAYERS):
            group_colours = (
                colours[idx]
                for colours in GROUP_COLOURS
            )
            for move, colours in zip(similar_moves, group_colours):
                    create_convergence_plot(iterations, x, player, move, colours)
        plt.title(f"Convergence of {title} Over Time")
        plt.tight_layout()
        plt.savefig(f"{title.lower().replace(' ', '-')}-convergence-plot")
        plt.clf()

if __name__ == '__main__':
    main()
