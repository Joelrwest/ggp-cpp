"""
File that takes in a benchmark file and outputs plots for scissor paper rock game.

All plots written using:
https://www.geeksforgeeks.org/how-to-plot-a-time-series-in-matplotlib/
"""

import matplotlib.pyplot as plt
import json

# Can change the input files, the time taken, and the colours used from these top constants

OLD_FILENAME = 'old.json'
NEW_FILENAME = 'new.json'

OLD_TIME_FILTER_MS = 600
NEW_TIME_FILTER_MS = 20
COMBINED_TIME_FILTERS_MS = (
    OLD_TIME_FILTER_MS,
    NEW_TIME_FILTER_MS,
    (OLD_TIME_FILTER_MS + NEW_TIME_FILTER_MS) / 2,
)

NEW_PREFIX = 'new'
OLD_PREFIX = 'old'
COMBINED_PREFIX = 'combined'

OLD_COLOURS = (
    'red',
    'tomato',
)
NEW_COLOURS = (
    'limegreen',
    'darkgreen',
)

X_AXIS_NAME = 'Time (ms)'

IDEAL_COLOUR = 'dimgrey'
IDEAL_STYLE = ':'

X_TICKS_ROTATION = 30

# End of changable constants

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

TOTAL_GAME_REWARD = 100

def get_time_data_ms(iterations, filter_after_ms = None):
    time_data_ms = [
        iteration[TIME_KEY] / 1e3
        for iteration in iterations
    ]

    if filter_after_ms is not None:
        time_data_ms = [
            time
            for time in time_data_ms
            if time <= filter_after_ms
        ]

    return time_data_ms

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
    return TOTAL_GAME_REWARD - opponent_ev # Can be done this way since it's a 0 sum game

def create_ev_data(iterations, x: list[float], calculate_ev_function, colours: tuple[str]) -> None:
    iterations = iterations[:len(x)]

    y_1 = [
        calculate_ev_function(iteration[PROBABILITIES_KEY], PLAYER_1)
        for iteration in iterations
    ]

    y_2 = [
        calculate_ev_function(iteration[PROBABILITIES_KEY], PLAYER_2)
        for iteration in iterations
    ]

    plt.plot(x, y_1, color=colours[0])
    plt.plot(x, y_2, color=colours[1])

    plt.xticks(rotation=X_TICKS_ROTATION, ha='right')

    plt.xlabel(X_AXIS_NAME)

    plt.tight_layout()

def create_non_adapting_ev_plot(iterations, x: list[float], colours: tuple[str]) -> None:
    create_ev_data(iterations, x, calculate_non_adapting_ev, colours)

    plt.ylabel('Expected Value')
    plt.title('Expected Value Against Non-Adapting Optimal Opponent')

def create_adapting_ev_plot(iterations, x: list[float], colours: tuple[str]) -> None:
    create_ev_data(iterations, x, calculate_adapting_ev, colours)

    y_3 = [
        TOTAL_GAME_REWARD / 2
        for _ in iterations[:len(x)]
    ]

    plt.plot(x, y_3, color=IDEAL_COLOUR, linestyle=IDEAL_STYLE)

    plt.ylabel('Expected Value')
    plt.title('Expected Value Against Adapting Optimal Opponent')

def create_convergence_plot(iterations, x: list[float], move: str, colours: tuple[str]):
    iterations = iterations[:len(x)]

    y_1 = [
        iteration[PROBABILITIES_KEY][MOVES_TO_INPUTS[PLAYER_1][move]]
        for iteration in iterations
    ]

    y_2 = [
        iteration[PROBABILITIES_KEY][MOVES_TO_INPUTS[PLAYER_2][move]]
        for iteration in iterations
    ]

    y_3 = [
        IDEAL_PROBABILITIES[move]
        for _ in iterations
    ]

    plt.plot(x, y_1, color=colours[0])
    plt.plot(x, y_2, color=colours[1])
    plt.plot(x, y_3, color=IDEAL_COLOUR, linestyle=IDEAL_STYLE)

    plt.xticks(rotation=X_TICKS_ROTATION, ha='right')

    plt.xlabel(X_AXIS_NAME)
    plt.ylabel('Probability of Playing')
    plt.title(f"Convergence of {move.capitalize().replace(' ', '-')} Over Time")

    plt.tight_layout()

def main() -> None:
    with open(OLD_FILENAME, 'r') as file:
        iterations_old = json.loads(file.read())[ITERATIONS_KEY]
    with open(NEW_FILENAME, 'r') as file:
        iterations_new = json.loads(file.read())[ITERATIONS_KEY]

    x_old = get_time_data_ms(iterations_old, OLD_TIME_FILTER_MS)
    x_new = get_time_data_ms(iterations_new, NEW_TIME_FILTER_MS)

    # Create individual plots
    for x, iterations, colours, prefix in ((x_old, iterations_old, OLD_COLOURS, OLD_PREFIX), (x_new, iterations_new, NEW_COLOURS, NEW_PREFIX)):
        create_non_adapting_ev_plot(iterations, x, colours)
        plt.savefig(f"{prefix}-non-adapting-ev-plot")
        plt.clf()

        create_adapting_ev_plot(iterations, x, colours)
        plt.savefig(f"{prefix}-adapting-ev-plot")
        plt.clf()

        for move in (ROCK, PAPER, SCISSORS):
            create_convergence_plot(iterations, x, move, colours)
            plt.savefig(f"{prefix}-{move.lower()}-convergence-plot")
            plt.clf()

    # Created combined plots
    for count, combined_time_filter_ms in enumerate(COMBINED_TIME_FILTERS_MS):
        x_old = get_time_data_ms(iterations_old, combined_time_filter_ms)
        x_new = get_time_data_ms(iterations_new, combined_time_filter_ms)
        prefix = f"{COMBINED_PREFIX}-{count}"

        create_non_adapting_ev_plot(iterations_old, x_old, OLD_COLOURS)
        create_non_adapting_ev_plot(iterations_new, x_new, NEW_COLOURS)
        plt.savefig(f"{prefix}-non-adapting-ev-plot")
        plt.clf()

        create_adapting_ev_plot(iterations_old, x_old, OLD_COLOURS)
        create_adapting_ev_plot(iterations_new, x_new, NEW_COLOURS)
        plt.savefig(f"{prefix}-adapting-ev-plot")
        plt.clf()

        for move in (ROCK, PAPER, SCISSORS):
            create_convergence_plot(iterations_old, x_old, move, OLD_COLOURS)
            create_convergence_plot(iterations_new, x_new, move, NEW_COLOURS)
            plt.savefig(f"{prefix}-{move.lower()}-convergence-plot")
            plt.clf()

if __name__ == '__main__':
    main()
