cmake_minimum_required(VERSION 3.26)
project(supermarket-1cash C)

set(CMAKE_C_STANDARD 11)

add_executable(saloha-full 
saloha.c cues.c cra.c protocol.c stats.c stochastic.c 
saloha.h cues.h cra.h stats.h )
