#! /bin/bash

find . \( -path ./_deps -o -path ./CMakeFiles \) -prune -o \( -iname '*.hpp' -o -iname '*.cpp' \) | xargs clang-format -style=Microsoft -i
