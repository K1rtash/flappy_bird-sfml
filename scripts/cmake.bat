@echo off

cd ..
cmake -B build
cmake --build build

call "build/flappy_bird_sfml.exe"