#!/bin/bash
g++ -std=c++20 -O3 -Wall -Wextra -Wpedantic -oapp ray_intersection_test.cpp src/engine/camera.cpp main.cpp -Isrc/ -Ithird/ && ./app
