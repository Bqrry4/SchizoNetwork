cmake_minimum_required(VERSION 3.27)
project(SI_Proiect C)

set(CMAKE_C_STANDARD 11)

include_directories(src/algorithms/RC6)

add_executable(SI_Proiect
        src/algorithms/RC6/RC6.c
        src/algorithms/RC6/RC6.h)
