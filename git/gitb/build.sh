#!/bin/bash

g++ -g -std=c++1z -o gitb src/main.cpp src/application.cpp src/git.cpp src/util.cpp -lform -lncurses
