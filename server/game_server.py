from propnet.propnet_wrapper import PropnetWrapper, RANDOM_ROLE

from argparse import ArgumentParser
from http.server import BaseHTTPRequestHandler, HTTPServer
import random
import json

HOST = "localhost"
PORT = 8080
URL = f"http://{HOST}:{PORT}"
MAX_GAMES = int(5e4)

def main() -> None:
    argument_parser = ArgumentParser()
    argument_parser.add_argument(
        '-g',
        dest='game',
        required=True,
        type=str,
        help='Game to be played.'
    )
    argument_parser.add_argument(
        '-n',
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

    print(f"Playing {game} {num_games} times")

    server = create_server(game, num_games)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        server.server_close()

def create_server(game: str, num_games: int) -> HTTPServer:
    shared_state = GameServerSharedState(game, num_games)
    def create_handler(*args):
        return GameServerHandler(shared_state, *args)

    server = HTTPServer((HOST, PORT), create_handler)
    print(f"Created game server at {URL}")
    return server

class GameServerSharedState:
    def __init__(self, game: str, num_games: int):
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
        self.websocket = None

class GameServerHandler(BaseHTTPRequestHandler):
    def __init__(self, shared_state: GameServerSharedState, *args):
        self.shared_state = shared_state
        BaseHTTPRequestHandler.__init__(self, *args)

    def respond(self, code: int, message: str) -> None:
        self.send_response(code)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(message.encode('utf-8'))

    def do_POST(self) -> None:
        try:
            body_size = int(self.headers.get('content-length'))
            body = self.rfile.read(body_size)
            json_body = json.loads(body)

            role = json_body['role']
            input = int(json_body['input'])
        except KeyError:
            self.respond(400, 'Unexpected body format. Expected two keys: input and role')
            return
        except ValueError:
            self.respond(400, 'Expected input as an integer')
            return

        if role in self.shared_state.inputs:
            self.respond(409, f"Already received an input for {role}")
            return

        if role not in self.shared_state.propnet_wrapper.get_roles():
            self.respond(409, f"{role} is not a role for the current game")
            return

        self.respond(200, f"Received input {input} for {role}")

        self.shared_state.inputs[role] = input

        if len(self.shared_state.inputs) == self.shared_state.propnet_wrapper.num_roles():
            if self.shared_state.propnet_wrapper.is_randomness():
                legals = self.shared_state.propnet_wrapper.get_legals_for(RANDOM_ROLE)
                legal = random.choice(legals)
                self.inputs.append(legal)

            # Make moves
            self.inputs = [
                input
                for _, input in self.inputs
            ]
            self.shared_state.propnet_wrapper.take_inputs(self.inputs)
            if self.shared_state.propnet_wrapper.is_terminal():
                # Done with this game
                self.log_stats()
                self.shared_state.propnet_wrapper.reset_data()

                self.shared_state.remaining_games -= 1
                if self.shared_state.remaining_games == 0:
                    print("Done playing!")
                    exit(0)

            # Broadcast the inputs made

            print('clearing')
            self.inputs.clear()

    def log_stats(self) -> None:
        scores = self.shared_state.propnet_wrapper.get_scores()

        for role in self.shared_state.propnet_wrapper.get_roles():
            self.shared_state.cumulative_scores[role] += scores[role]

        winning_score = max(scores.values())
        winning_roles = [
            role
            for role, score in scores
            if score == winning_score
        ]
        num_winning_roles = len(winning_roles)
        for role in winning_roles:
            self.shared_state.cumulative_wins[role] += (1.0 / num_winning_roles)

if __name__ == '__main__':
    main()
