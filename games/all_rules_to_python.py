"""
Inspired from original propnet from Zac Patridge in deep-hi-ggp.

Takes in a GDL-II file and converts into a Python propositional network.
"""

import os

from folder_constants import RULES_FOLDER, PYTHON_FOLDER

VALID_EXTENSIONS = ['gdl', 'kif']
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

def main() -> None:
    files = (
        file for file in os.listdir(RULES_FOLDER)
        if any(
            file.endswith(f".{extension}")
            for extension in VALID_EXTENSIONS
        )
    )

    for file in files:
        game = file
        for extension in VALID_EXTENSIONS:
            game = game.removesuffix(f".{extension}")

        input_path = os.path.join(RULES_FOLDER, file)
        output_path = os.path.join(PYTHON_FOLDER, f"{game}.py")
        to_propnet(input_path, output_path)

def to_propnet(input_path: str, output_path: str) -> None:
    print(f"Processing {input_path} into {output_path}")
    # TODO: Doesn't run yet... Unsure of what/where propnet_convert.Convert is
    command = f"/usr/bin/java -Dfile.encoding=UTF-8 -classpath \"{GGP_BASE_SUBCOMMANDS}\" propnet_convert.Convert {input_path} {output_path}"
    print(f"\n{command}\n")

    os.chdir(GGP_BASE_DIR)
    os.system(command)

if __name__ == '__main__':
    main()
