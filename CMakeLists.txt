# 1. Minimum version requirement
cmake_minimum_required(VERSION 3.20)

# 2. Project name
project(dispatch)

# 3. Use C++17 or C++20 (Modern C++ is better!)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. Create the executable
# Syntax: add_executable(ProgramName source_files...)
add_executable(my_program
        main.cpp
        CommandProcessor.cpp
        CommandProcessor.h
        ConfigManager.cpp
        ConfigManager.h
)