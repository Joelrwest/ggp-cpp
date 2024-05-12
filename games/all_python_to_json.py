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
    type = entry[2]

    # Repetitive, but does the job
    if type == TRANSITION:
        extra_fields = {
            'type_name': 'TRANSITION',
            'ins': entry[3],
            'outs': entry[4],
        }
    elif type in (AND, OR):
        extra_fields = {
            'type_name': 'AND' if type == AND else 'OR',
            'ins': entry[3],
            'outs': entry[4],
        }
    elif type == NOT:
        ins = entry[3]
        num_ins = len(ins)
        if num_ins != 1:
            logging.error(f"For proposition of id = {id}, num_ins = {num_ins}")
            exit(1)

        extra_fields = {
            'type_name': 'NOT',
            'in': ins[0],
            'outs': entry[4],
        }
    elif type == PROPOSITION:
        extra_fields = {
            'type_name': 'PROPOSITION',
            'ins': entry[3],
            'outs': entry[4],
            'proposition_type': entry[5],
            'gdl': entry[6],
        }
    elif type == CONSTANT:
        extra_fields = {
            'type_name': 'CONSTANT',
            'value': entry[1],
        }
    else:
        logging.error(f"type = {type} is unknown")
        exit(1)

    return {
        'id': entry[0],
        'type': type,
    } | extra_fields

if __name__ == '__main__':
    main()
