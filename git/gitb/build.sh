#!/bin/bash

DEFINES="-DCOLOR"

g++ -g ${DEFINES} -std=c++1z -o gitb src/main.cpp src/application.cpp src/git.cpp -lform -lncurses
