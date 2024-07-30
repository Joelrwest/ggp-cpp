#! /bin/bash

sudo apt update
sudo apt upgrade
sudo apt install curl cmake build-essential unzip
curl -L https://download.pytorch.org/libtorch/cu124/libtorch-cxx11-abi-shared-with-deps-2.4.0%2Bcu124.zip -o libtorch.zip # May not be the correct package all the time
unzip libtorch.zip -d ~
rm libtorch.zip
cmake .
make
