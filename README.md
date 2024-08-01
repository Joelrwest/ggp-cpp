# Optimising Deep Learning and Search for Imperfect-Information Games in General Game Playing

An [imperfect-information general game playing](https://en.wikipedia.org/wiki/General_game_playing) codebase written in C++ which utilises deep learning and CFR search.

Done as part of a bachelors thesis under Prof. Michael Thielscher at UNSW.
Goal was to extend previous works [Hidden Information General Game
Playing with Deep Learning and Search](https://cgi.cse.unsw.edu.au/~mit/Papers/PRICAI22.pdf) by Zachary Partridge (also under Michael Thielscher at UNSW).
See the [corresponding paper](TODO: Link) for more details.

## Installation

Start by cloning and moving into this repository:

```bash
git clone https://github.com/Joelrwest/ggp-cpp.git
cd ggp-cpp
```

If using an Ubuntu machine and common C++ tools aren't setup, they can be installed using:

```bash
sudo apt update
sudo apt upgrade
sudo apt install curl cmake build-essential unzip gdb
```

Next, ensure that the correct version of LibTorch for C++ is installed, which can be found [here](https://pytorch.org/get-started/locally/).
This project expects it to be downloaded to `~`, but this can be changed via the `Torch_DIR` variable in [CMakeLists.txt](CMakeLists.txt) and [player/CMakeLists.txt](player/CMakeLists.txt).
For example, as of writing this the ***CPU-only*** version of LibTorch can be installed via:

```bash
curl -L https://download.pytorch.org/libtorch/nightly/cpu/libtorch-shared-with-deps-latest.zip -o libtorch.zip # Download
unzip libtorch.zip -d ~ # Unzip
rm libtorch.zip # Cleanup
```

Other versions of LibTorch can be found [here](https://pytorch.org/).
Next pull in other non-machine specific dependancies, setup CMake and compile using:

```bash
cmake . && make
```

## Training

Models can be trained via the `train` executable.
See `./train --help` for detailed options.
For example, to train a model for Blind Tic-Tac-Toe using 10 concurrent games (and otherwise default parameters) use:

```bash
./train --game blindtictactoe --concurrent-games 10
```

Models are then outputted to `models/<game>/`.
To load models within code call `player::Model::load_game_number`.
Note that pretrained models contained in this repository are trained on CUDA and thus don't work for CPU only versions of LibTorch.

## Repository structure

* Code is seperated into modules
    + `agents/` contains code related to the base agent which all other agents are derived from (including the player) and simple agents (e.g., human, random, plays first, plays last).
    + `player/` all code for the player including CFR, samplers and the player itself.
    + `propnet/` code for the propositional network including parsing, roles, state and propositional network itself.
    + `server/` webserver to allow a common interface for players each using a propositional network to play against each other.
    + `src/` source code for the actual executables and common simple setup functionality.
* Data/reports
    + `benchmarks/` contains benchmarks outputted from `benchmark` executable, plotting programs written in Python and plots.
    + `documents/` final presentation and report created as part of this work.
    + `games/` folder containing the rules (as GDL-II files), Python propositional network files and JSON propositional network files. Additionally contains Python script to convert Python propositional network files to JSON. Note that the script to create Python propositional network files from GDL-II is unfinished.
    + `models/` contains pre-trained models. Note that these are created using a CUDA machine and thus don't run on CPU-only versions of LibTorch.
* Shell scripts
    + `benchmark.sh` runs `benchmark` executable on given game with highest priority (on a Linux machine).
    + `lint.sh` recursively lints all C++ files in repository.
    + `remote-install.sh` installs everything required to both compile and run program on Ubuntu machine. Note that the script assumes CUDA 12.1 is the required version of LibTorch. Tested on both AWS and multiple Vast.ai machines.
