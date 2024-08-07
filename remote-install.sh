#! /bin/bash

echo -e "\033[1;91mWarning: \033[1;33mThe version of LibTorch installed by this script is for \033[4;33mCUDA 12.1\033[0m\033[1;33m."
echo -e "Exit within the next 10 seconds and change the download URL if a different version is required."
sleep 10

echo -e "Now installing...\033[0m"
sudo apt update
sudo apt upgrade
sudo apt install curl cmake build-essential unzip sysbench gdb
curl -L https://download.pytorch.org/libtorch/cu121/libtorch-cxx11-abi-shared-with-deps-2.4.0%2Bcu121.zip -o libtorch.zip # May not be the correct package all the time
unzip libtorch.zip -d ~
rm libtorch.zip
cmake .
make
