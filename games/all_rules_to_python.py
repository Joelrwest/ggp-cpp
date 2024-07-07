"""
Inspired from original propnet from Zac Patridge in deep-hi-ggp.

Takes in a GDL-II file and converts into a C++ propositional network.

TODO: Cannot find original function propnet_convert.Convert is...
"""

import os
import argparse

GGP_BASE_DIR = os.environ['GGP_BASE_DIR']

SUBCOMMANDS = (
    'bin',
    'lib/JUnit/junit-4.11.jar',
    'lib/JUnit/hamcrest-core-1.3.jar',
    'lib/Guava/guava-14.0.1.jar',
    'lib/Jython/jython.jar',
    'lib/Clojure/clojure.jar',
    'lib/Batik/batik-1.7.jar',
    'lib/FlyingSaucer/core-renderer.jar',
    'lib/javassist/javassist.jar',
    'lib/reflections/reflections-0.9.9-RC1.jar',
    'lib/Htmlparser/htmlparser-1.4.jar',
    'lib/gdl-validation/gdl-validation-0.2.2.jar',
    'lib/gdl-validation/java-cup-11b-runtime-2015.03.26.jar',
)

GGP_BASE_SUBCOMMANDS = ':'.join((
    os.path.join(GGP_BASE_DIR, subcommand)
    for subcommand in SUBCOMMANDS
))


def to_propnet(game: str, file: str) -> None:
    output = os.path.abspath(
        os.path.join('propnet', f"{game}.hpp")
    )
    # TODO: Doesn't run yet... Unsure of what/where propnet_convert.Convert is
    command = f"/usr/bin/java -Dfile.encoding=UTF-8 -classpath \"{GGP_BASE_SUBCOMMANDS}\" propnet_convert.Convert {file} {output}"
    print(f"\n{command}\n")

    os.chdir(GGP_BASE_DIR)
    os.system(command)

def get_game() -> str:
    arg_parser = argparse.ArgumentParser()

    arg_parser.add_argument(
        '-g',
        dest='game',
        required=True,
        type=str,
        help='Convert {game}.gdl to a C++ propnet.'
    )

    args = arg_parser.parse_args()
    return args.game

if __name__ == '__main__':
    game = get_game()
    file = os.path.abspath(f"rules/{game}.gdl")
    to_propnet(game, file)
