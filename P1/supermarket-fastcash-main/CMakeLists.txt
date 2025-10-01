# ------------------------------------------------------------------------------
# Supermarket n-Cashier Case C Project
# ------------------------------------------------------------------------------
# CMakeLists.txt
# CMake configuration for the "supermarket_n_cashier" C project
#
# Purpose:
#   - Create the supermarket-1cash executable from source files in src/
#
# Structure:
#   - src/    → Contains the codes separated with modules 2 modules (define as many modules as team members)
#   - logs/  →  Input and Output of script runs
#
# ------------------------------------------------------------------------------

# CMake top-level build configuration
cmake_minimum_required(VERSION 3.10)

project(supermarket_n_cashier_simulation C)
message(STATUS "(${PROJECT_NAME}) Configuring supermarket_n_cashier C project...")

# Set language standard
# Set the required C standard to C11: required to use static_assert and other modern features.
set(CMAKE_C_STANDARD 11) 
set(CMAKE_C_STANDARD_REQUIRED ON)
message(STATUS " - (${PROJECT_NAME}) C standard set to C11")

# Collect all .c files inside src/
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# Create Executable 
add_executable(supermarket ${SOURCES})

# Rename the output executable
set_target_properties(supermarket PROPERTIES OUTPUT_NAME "supermarket-ncash")
message(STATUS " - (${PROJECT_NAME}) created executable 'supermarket-ncash.exe'")

# Include headers (they’re all in src/)
target_include_directories(supermarket PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add useful compiler warnings (MSVC vs GCC/Clang)
if (MSVC)
    target_compile_options(supermarket PRIVATE /W4 /permissive-)
else()
    target_compile_options(supermarket PRIVATE -Wall -Wextra -pedantic -g)
endif()

