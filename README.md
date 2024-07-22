# Optimising Deep Learning and Search for Imperfect-Information Games in General Game Playing

An [imperfect-information general game playing](https://en.wikipedia.org/wiki/General_game_playing) codebase written in C++ which utilises deep learning and CFR search.
Codebase can be seperated into 2 distinct sections:
- Propositional Network: TODO
- Player: TODO

Done as part of a bachelors thesis under Prof. Michael Thielscher at UNSW.
Goal was to extend previous works [Hidden Information General Game
Playing with Deep Learning and Search](https://cgi.cse.unsw.edu.au/~mit/Papers/PRICAI22.pdf) by Zachary Partridge (also under Michael Thielscher at UNSW).
See the corresponding paper (TODO: Link) for more details.

## Installation

Start by cloning and moving into this repository:

```bash
git clone git@github.com:Joelrwest/ggp-cpp.git
cd ggp-cpp
```

Next, ensure that the correct version of LibTorch for C++ is installed, which can be found [here](https://pytorch.org/get-started/locally/).
This project expects it to be downloaded to `~`, but this can be changed via the `Torch_DIR` variable in [CMakeLists.txt](CMakeLists.txt) and [player/CMakeLists.txt](player/CMakeLists.txt).
For example, as of writing this the ***CPU-only*** version of LibTorch can be installed via:

```bash
wget https://download.pytorch.org/libtorch/nightly/cpu/libtorch-shared-with-deps-latest.zip # Download
unzip libtorch-shared-with-deps-latest.zip -d ~ # Unzip
rm libtorch.zip # Cleanup
```

Next pull in other non-machine specific dependancies and setup CMake via:

```bash
cmake .
```

And simply compile using:

```bash
make
```

## Training

TODO

## Playing

TODO

## Pretrained models

TODO

## Code structure

TODO
