"""
Modelled off of tutorial:
https://websockets.readthedocs.io/en/stable/intro/tutorial1.html

If it's not already completely obvious I have no idea what I'm doing here :D
I'm sure there's a much simpler/cleaner way of doing all of this
"""

from propnet.propnet_wrapper import PropnetWrapper, RANDOM_ROLE

from argparse import ArgumentParser
from time import sleep
import asyncio
import json
import websockets
import random
import os

HOST = 'localhost'
PORT = 8080
MAX_GAMES = int(5e4)

TYPE_KEY = 'type'
ROLE_KEY = 'role'
GAME_KEY = 'game'
INPUT_KEY = 'input'
PROGRAM_KEY = 'program'

NEW_CONNECTION_MESSAGE_TYPE = 'connection'
INPUT_MESSAGE_TYPE = 'input'
ERROR_MESSAGE_TYPE = 'error'
UPDATE_MESSAGE_TYPE = 'update'
TERMINAL_MESSAGE_TYPE = 'terminal'
FINISHED_MESSAGE_TYPE = 'finished'

state = None

class State:
    def __init__(self, game: str, num_games: int):
        self.game = game
        self.remaining_games = num_games

        self.propnet_wrapper = PropnetWrapper(game)
        self.remaining_games: int = num_games

        self.inputs: dict[str, int] = {}
        self.cumulative_scores: dict[str, int] = {
            role: 0
            for role in self.propnet_wrapper.get_roles()
        }
        self.cumulative_wins: dict[str, float] = {
            role: 0
            for role in self.propnet_wrapper.get_roles()
        }
        self.connected_roles = {}
        self.websockets = []

def make_error(message: str) -> str:
    print(message)
    return json.dumps({
        TYPE_KEY: ERROR_MESSAGE_TYPE,
        'message': message,
    })

def get_command_line_arguments() -> tuple[str, int]:
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
        '-n',
        '--num_games',
        dest='num_games',
        required=False,
        type=int,
        default=MAX_GAMES,
        help='Number of games to be played.'
    )
    arguments = argument_parser.parse_args()

    game: str = arguments.game
    num_games: int = arguments.num_games
    if num_games < 1:
        print(f"Must play at least 1 game (given {num_games})")
        exit(1)

    return game, num_games

async def new_connection(websocket, event) -> None:
    try:
        new_connection_game = event[GAME_KEY]
        role = event[ROLE_KEY]
        program = event[PROGRAM_KEY]
    except KeyError:
        error = make_error(f"Unexpected body format. Expected keys {GAME_KEY}, {PROGRAM_KEY} and {ROLE_KEY}")
        await websocket.send(error)

    if new_connection_game != state.game:
        error = make_error(f"Connection request was for a game {new_connection_game} not currently being played (expected {state.game})")
        await websocket.send(error)

    if role not in state.propnet_wrapper.get_roles():
        error = make_error(f"Connection request was for a role {role} that doesn't exist in {state.game}")
        await websocket.send(error)

    if role in state.connected_roles:
        error = make_error(f"Role {role} already has a connection")
        await websocket.send(error)

    state.connected_roles[role] = program

async def input(websocket, event) -> None:
    try:
        role = event[ROLE_KEY]
        input = int(event[INPUT_KEY])
    except KeyError:
        error = make_error(f"Unexpected body format. Expected keys {INPUT_KEY} and {ROLE_KEY}")
        await websocket.send(error)
    except ValueError:
        error = make_error(f"Expected input as an integer")
        await websocket.send(error)

    if role in state.inputs:
        error = make_error(f"Already received an input for {role}")
        await websocket.send(error)

    if role not in state.connected_roles:
        error = make_error(f"{role} is not a connected role")
        await websocket.send(error)

    state.inputs[role] = input

    if len(state.inputs) == state.propnet_wrapper.num_roles():
        if state.propnet_wrapper.is_randomness():
            legals = state.propnet_wrapper.get_legals_for(RANDOM_ROLE)
            legal = random.choice(legals)
            state.inputs.append(legal)

        # Make moves
        state.propnet_wrapper.take_inputs([
            input
            for input in state.inputs.values()
        ])

        # Broadcast the inputs made
        update = json.dumps({
            TYPE_KEY: UPDATE_MESSAGE_TYPE,
            'inputs': [input for input in state.inputs.values()],
        })
        for websocket in state.websockets:
            await websocket.send(update)
        state.inputs.clear()

        if state.propnet_wrapper.is_terminal():
            terminal = json.dumps({
                TYPE_KEY: TERMINAL_MESSAGE_TYPE,
            })
            for websocket in state.websockets:
                await websocket.send(terminal)

            # Done with this game
            scores = state.propnet_wrapper.get_scores()

            for role in state.propnet_wrapper.get_roles():
                state.cumulative_scores[role] += scores[role]

            winning_score = max(scores.values())
            winning_roles = [
                role
                for role, score in scores.items()
                if score == winning_score
            ]
            num_winning_roles = len(winning_roles)
            for role in winning_roles:
                state.cumulative_wins[role] += (1.0 / num_winning_roles)
            state.propnet_wrapper.reset_data()

            state.remaining_games -= 1

async def handler(websocket) -> None:
    state.websockets.append(websocket)
    async for message in websocket:
        event = json.loads(message)
        try:
            type = event[TYPE_KEY]
        except KeyError:
            error = make_error(f"Expected key {TYPE_KEY} to indicate message type")
            await websocket.send(error)

        if type == NEW_CONNECTION_MESSAGE_TYPE:
            await new_connection(websocket, event)
        elif type == INPUT_MESSAGE_TYPE:
            await input(websocket, event)
        else:
            error = make_error(f"Unknown type {type}")
            await websocket.send(error)

        if state.remaining_games == 0:
            print('Done playing!')
            directory_path = os.path.join('..', 'benchmarks', state.game)
            os.makedirs(directory_path, exist_ok=True)
            players_vs = '-vs-'.join([
                f"{role}-{program}"
                for role, program in state.connected_roles.items()
            ])
            file_name = f"strength-{players_vs}"
            file_path = os.path.join(directory_path, file_name)
            with open(file_path, 'w') as file:
                file.write(json.dumps({
                    'cumulative_scores': state.cumulative_scores,
                    'cumulative_wins': state.cumulative_wins,
                }))
            finished = json.dumps({
                TYPE_KEY: FINISHED_MESSAGE_TYPE,
            })
            for websocket in state.websockets:
                await websocket.send(finished)

            sleep(5)
            exit(0)

async def main() -> None:
    global state
    game, num_games = get_command_line_arguments()
    state = State(game, num_games)
    print(f"Playing {game} {num_games} times")

    async with websockets.serve(handler, HOST, PORT):
        await asyncio.Future()

if __name__ == '__main__':
    asyncio.run(main())
