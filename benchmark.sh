#! /bin/bash

sudo nohup nice -n -20 ./benchmark --game $1 > $1.txt 2>&1 &
