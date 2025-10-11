cmake_minimum_required(VERSION 3.26)
project(rng C)

set(CMAKE_C_STANDARD 11)

add_executable(rng rng.h
        verif.c
        stochastic.c
        rng.c)
