cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
project(metal-cpp)


add_executable(metal-cpp main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/metal-cpp)
target_link_libraries(metal-cpp
        "-framework Metal"
        "-framework Foundation"
        "-framework QuartzCore"
        )
