"""
File that takes in a benchmark file and outputs plots for scissor paper rock game.

All plots written using:
https://www.geeksforgeeks.org/how-to-plot-a-time-series-in-matplotlib/
"""

from blindtictactoe_constants import *
import blindtictactoe.moves_to_inputs as normal
import blindtictactoeXbias.moves_to_inputs as biased
import blindtictactoeXwins.moves_to_inputs as winning

from argparse import ArgumentParser
import os
import matplotlib.pyplot as plt
import json

# Can change the input files, the time taken, and the colours used from these top constants

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

def get_time_data_h(iterations):
    return [
        iteration[TIME_KEY] / (60 * 60 * 1e6)
        for iteration in iterations
    ]

def get_iteration_num_data(iterations):
    return [
        i for i in range(len(iterations))
    ]

def create_convergence_plot(moves_to_inputs: dict, iterations, x: list[float], player: str, move: str, colour):
    y = [
        iteration[PROBABILITIES_KEY][moves_to_inputs[player][move]]
        for iteration in iterations
    ]

    plt.plot(x, y, color=colour)

    plt.ylabel('Probability')

def parse_command_line() -> tuple[str, str]:
    argument_parser = ArgumentParser()
    argument_parser.add_argument(
        '-g',
        '--game',
        dest='game',
        required=True,
        type=str,
        help='Game to be played.'
    )
    argument_parser.add_argument(
        '-f',
        '--file_name',
        dest='file_name',
        required=True,
        type=str,
        help='File containing data to plot.'
    )
    arguments = argument_parser.parse_args()

    game: str = arguments.game
    file_name: str = arguments.file_name

    return game, file_name

def save_figure(game: str, file_name: str) -> None:
    plt.savefig(os.path.join(game, file_name))

def main() -> None:
    game, file_name = parse_command_line()
    if game == normal.NAME:
        moves_to_inputs = normal.MOVES_TO_INPUTS
    elif game == biased.NAME:
        moves_to_inputs = biased.MOVES_TO_INPUTS
    elif game == winning.NAME:
        moves_to_inputs = winning.MOVES_TO_INPUTS
    else:
        print('Unknown variant of blindtictactoe')
        exit(1)

    players = [
        player
        for player in moves_to_inputs.keys()
    ]

    with open(os.path.join(game, file_name), 'r') as file:
        iterations = json.loads(file.read())[ITERATIONS_KEY]

    x_time = get_time_data_h(iterations)
    x_iteration_num = get_iteration_num_data(iterations)
    iterations = iterations[:len(x_time)]

    for player, colour in zip(players, SINGLE_COLOURS):
        for move in ALL_MOVES:
            create_convergence_plot(moves_to_inputs, iterations, x_time, player, move, colour)
            plt.xlabel(X_AXIS_NAME_TIME)
            plt.title(f"Convergence of {move.capitalize()} for {player} Over Time")
            plt.tight_layout()
            save_figure(game, f"{player}-{move.lower().replace(' ', '-')}-time-convergence-plot")
            plt.clf()

    for player, colour in zip(players, SINGLE_COLOURS):
        for move in ALL_MOVES:
            create_convergence_plot(moves_to_inputs, iterations, x_iteration_num, player, move, colour)
            plt.xlabel(X_AXIS_NAME_NUM_ITERATIONS)
            plt.title(f"Convergence of {move.capitalize()} for {player}")
            plt.tight_layout()
            save_figure(game, f"{player}-{move.lower().replace(' ', '-')}-iteration-num-convergence-plot")
            plt.clf()

    for idx, player in enumerate(players):
        for title, similar_moves in SIMILAR_MOVES:
            group_colours = [
                colours[idx]
                for colours in GROUP_COLOURS
            ]

            for move, colours in zip(similar_moves, group_colours):
                create_convergence_plot(moves_to_inputs, iterations, x_time, player, move, colours)
            plt.xlabel(X_AXIS_NAME_TIME)
            plt.title(f"Convergence of {title} for {player} Over Time")
            plt.tight_layout()
            save_figure(game, f"{player}-{title.lower().replace(' ', '-')}-time-convergence-plot")
            plt.clf()

            for move, colours in zip(similar_moves, group_colours):
                create_convergence_plot(moves_to_inputs, iterations, x_iteration_num, player, move, colours)
            plt.xlabel(X_AXIS_NAME_NUM_ITERATIONS)
            plt.title(f"Convergence of {title} for {player}")
            plt.tight_layout()
            save_figure(game, f"{player}-{title.lower().replace(' ', '-')}-iteration-num-convergence-plot")
            plt.clf()

    for title, similar_moves in SIMILAR_MOVES:
        for idx, player in enumerate(players):
            group_colours = [
                colours[idx]
                for colours in GROUP_COLOURS
            ]
            for move, colours in zip(similar_moves, group_colours):
                create_convergence_plot(moves_to_inputs, iterations, x_time, player, move, colours)
        plt.xlabel(X_AXIS_NAME_TIME)
        plt.title(f"Convergence of {title} Over Time")
        plt.tight_layout()
        save_figure(game, f"{title.lower().replace(' ', '-')}-time-convergence-plot")
        plt.clf()

        for idx, player in enumerate(players):
            group_colours = [
                colours[idx]
                for colours in GROUP_COLOURS
            ]
            for move, colours in zip(similar_moves, group_colours):
                create_convergence_plot(moves_to_inputs, iterations, x_iteration_num, player, move, colours)
        plt.xlabel(X_AXIS_NAME_NUM_ITERATIONS)
        plt.title(f"Convergence of {title}")
        plt.tight_layout()
        save_figure(game, f"{title.lower().replace(' ', '-')}-iteration-num-convergence-plot")
        plt.clf()

if __name__ == '__main__':
    main()
