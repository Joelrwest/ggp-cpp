import os
import importlib
import json
import logging

from constants import *

PYTHON_FOLDER = 'python'
JSON_FOLDER = 'json'

def main() -> None:
    games = (
        file.removesuffix('.py')
        for file in os.listdir(PYTHON_FOLDER)
        if file.endswith('.py')
    )

    for game in games:
        process_game(game)

def process_game(game: str) -> None:
    module = importlib.import_module(f"{PYTHON_FOLDER}.{game}")
    mapped_entries = list(map(map_entry, module.entries))
    dict = {
        'roles': module.roles,
        'entries': mapped_entries,
    }

    json_file = f"{os.path.join(JSON_FOLDER, game)}.json"
    with open(json_file, 'w') as file:
        file.write(json.dumps(dict, indent = 4))

def map_entry(entry):
    id = entry[0]
    python_type = entry[2]

    # Repetitive, but does the job
    if python_type == TRANSITION:
        return {
            'id': id,
            'type': 'TRANSITION',
            'ins': entry[3],
            'outs': entry[4],
        }
    elif python_type in (AND, OR):
        type = 'AND' if python_type == AND else 'OR'
        return {
            'id': id,
            'type': type,
            'ins': entry[3],
            'outs': entry[4],
        }
    elif python_type == NOT:
        ins = entry[3]
        num_ins = len(ins)
        if num_ins != 1:
            logging.error(f"For proposition of id = {id}, num_ins = {num_ins}")
            exit(1)

        return {
            'id': id,
            'type': 'NOT',
            'in': ins[0],
            'outs': entry[4],
        }
    elif python_type == PROPOSITION:
        return {
            'id': id,
            'type': 'PROPOSITION',
            'ins': entry[3],
            'outs': entry[4],
            'proposition_type': entry[5],
            'gdl': entry[6],
        }
    elif python_type == CONSTANT:
        return {
            'id': id,
            'type': 'CONSTANT',
            'value': entry[1],
        }
    else:
        logging.error(f"python_type = {python_type} is unknown")
        exit(1)

if __name__ == '__main__':
    main()
