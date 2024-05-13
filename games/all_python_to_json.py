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
    print(f"Processing {game}")
    module = importlib.import_module(f"{PYTHON_FOLDER}.{game}")
    mapped_entries = list(map(map_entry, module.entries))

    # Split the transitions apart into
    # pre and post transitions
    split_transitions(mapped_entries)

    dict = {
        'roles': module.roles,
        'entries': mapped_entries,
    }

    # Check that every id is taken
    if (
        any(entry['id'] < 0 for entry in dict['entries']) or
        get_max_id(dict['entries']) + 1 != len(dict['entries'])
    ):
        logging.error(f"Invalid ids for game {game}")
        exit(1)

    # Check that the random player is last
    if 'random' in dict['roles'] and 'random' != dict['roles'][-1]:
        logging.error(f"Random player is present and not in the final slot for {game}")
        exit(1)

    json_file = f"{os.path.join(JSON_FOLDER, game)}.json"
    with open(json_file, 'w') as file:
        file.write(json.dumps(dict, indent = 4))

def map_entry(entry) -> dict[str, list]:
    type = entry[2]

    # Repetitive, but does the job
    if type == TRANSITION:
        ins = entry[3]
        num_ins = len(ins)
        if num_ins != 1:
            logging.error(f"For proposition of id = {id}, num_ins = {num_ins}")
            exit(1)

        outs = entry[4]
        num_outs = len(outs)
        if num_outs != 1:
            logging.error(f"For proposition of id = {id}, num_outs = {num_outs}")
            exit(1)

        extra_fields = {
            'type_name': 'TRANSITION',
            'in': ins[0],
            'out': outs[0],
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
            'proposition_type': entry[5],
            'gdl': entry[6],
        }

        ins = entry[3]
        if extra_fields['proposition_type'] not in (init, input):
            if len(ins) != 1:
                logging.error(f"Expected a single in: {entry}")
                exit(1)
            extra_fields['in'] = ins[0]
        elif ins:
            logging.error(f"Missed ins: {entry}")
            exit(1)

        outs = entry[4]
        if extra_fields['proposition_type'] not in (goal, sees):
            extra_fields['outs'] = outs
        elif outs:
            logging.error(f"Missed outs: {entry}")
            exit(1)
    elif type == CONSTANT:
        if entry[1] == 0:
            logging.error(f"Found non true constant: {entry}")
            exit(1)

        extra_fields = {
            'type_name': 'CONSTANT',
        }
    else:
        logging.error(f"type = {type} is unknown")
        exit(1)

    return {
        'id': entry[0],
        'type': type,
    } | extra_fields

def split_transitions(entries: list[dict]) -> None:
    transitions = list(filter(lambda entry: entry['type'] == TRANSITION, entries))
    for pre_transition in transitions:
        post_transition = {
            'id': get_max_id(entries) + 1,
            'type': POST_TRANSITION,
            'type_name': 'POST TRANSITION',
            'in': pre_transition['id'],
            'out': pre_transition['out']
        }

        out = post_transition['out']
        if 'in' in entries[out]:
            entries[out]['in'] = post_transition['id']
        elif 'ins' in entries[out]:
            entries[out]['ins'] = [
                id if id != pre_transition['id'] else post_transition['id']
                for id in entries[out]['ins']
            ]
        else:
            logging.error('Tried to fix nodes outs where it wasn\'t present')
            exit(1)

        pre_transition['type'] = PRE_TRANSITION
        pre_transition['type_name'] = 'PRE TRANSITION'
        pre_transition['out'] = post_transition['id']

        entries.append(post_transition)

def get_max_id(entries: list[dict]) -> int:
    return max(
        entry['id']
        for entry in entries
    )

if __name__ == '__main__':
    main()
