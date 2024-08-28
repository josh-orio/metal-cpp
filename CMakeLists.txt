cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
project(metal-cpp)

add_subdirectory(metal-cmake)  # Metal - Library definition

add_executable(metal-cpp main.cpp)
target_link_libraries(metal-cpp METAL_CPP)