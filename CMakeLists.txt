cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
project(metal-cpp)

set(METAL_SOURCES math.metal) # include the project's metal files here
set(METAL_OUTPUT "default.metallib")

foreach (m_source ${METAL_SOURCES})
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/${m_source}
        COMMAND xcrun -sdk macosx metal -c ${CMAKE_SOURCE_DIR}/${m_source} -o ${CMAKE_BINARY_DIR}/${m_source}.air
    )
endforeach()

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/${METAL_OUTPUT}
    COMMAND xcrun -sdk macosx metallib ${CMAKE_BINARY_DIR}/*.air -o ${CMAKE_BINARY_DIR}/${METAL_OUTPUT}
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/*.air # air files are junk so get rid
    DEPENDS ${CMAKE_BINARY_DIR}/${METAL_SOURCES}
)

add_custom_target(default_library ALL DEPENDS ${CMAKE_BINARY_DIR}/${METAL_OUTPUT})

add_executable(metal-cpp main.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/metal-cpp)
target_link_libraries(metal-cpp
        "-framework Metal"
        "-framework Foundation"
        "-framework QuartzCore"
        )
