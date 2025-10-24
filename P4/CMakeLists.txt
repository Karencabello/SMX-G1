# ------------------------------------------------------------------------------
# Slotted-Aloha (saloha) C Project
# ------------------------------------------------------------------------------
# CMakeLists.txt
# CMake configuration for the "slotted_aloha" C project
#
# Purpose:
#   - Create the slotted_aloha executable from source files in src/
#
# Structure:
#   - src/     → Contains the code
#   - log/     → Input and Output of script runs
#   - CMakeLists.txt → This file (to run with CMake and VSCode)
#   - build/   → Build output directory (created by CMake if you run with VSCode)
#   - .vscode/ → VSCode configuration files
# ------------------------------------------------------------------------------

# CMake top-level build configuration
cmake_minimum_required(VERSION 3.10)

project(saloha-simulation C)
message(STATUS "(${PROJECT_NAME}) Configuring saloha-simulation C project...")

# Set language standard
# Set the required C standard to C11: required to use static_assert and other modern features.
set(CMAKE_C_STANDARD 11) 
set(CMAKE_C_STANDARD_REQUIRED ON)
message(STATUS " - (${PROJECT_NAME}) C standard set to C11")

# Collect all .c files inside src/
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# Create Executable 
add_executable(saloha ${SOURCES})

# Rename the output executable
set_target_properties(saloha PROPERTIES OUTPUT_NAME "saloha")
message(STATUS " - (${PROJECT_NAME}) created executable 'saloha.exe'")

# Include headers (they’re all in src/)
target_include_directories(saloha PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add useful compiler warnings (MSVC vs GCC/Clang)
if (MSVC)
    target_compile_options(saloha PRIVATE /W4 /permissive-)
else()
    target_compile_options(saloha PRIVATE -Wall -Wextra -pedantic -g)
endif()

