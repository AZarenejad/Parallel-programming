#!/bin/bash
g++ part1.cpp -o part1 -std=c++11 -L /opt/intel/ipp/lib/ -I /opt/intel/ipp/include/ -lippcore -msse3 -lpthread
g++ part2.cpp -o part2 -std=c++11 -L /opt/intel/ipp/lib/ -I /opt/intel/ipp/include/ -lippcore -msse3 -lpthread
