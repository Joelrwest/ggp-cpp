#! /bin/bash

sudo apt update
sudo apt install cmake build-essential unzip
wget https://download.pytorch.org/libtorch/nightly/cpu/libtorch-shared-with-deps-latest.zip
unzip libtorch-shared-with-deps-latest.zip -d ~
rm libtorch-shared-with-deps-latest.zip
git clone git@github.com:Joelrwest/ggp-cpp.git
cd ggp-cpp
cmake .
make
