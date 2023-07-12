cmake_minimum_required(VERSION 3.1)

project(FluidSimulation)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O3")

add_executable(FluidSimulation main.cpp verlet.hpp solver.hpp renderer.hpp collisionGrid.hpp utils/colors.hpp)

include_directories(/usr/local/include)

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
include_directories(${SFML_INCLUDE_DIRS})
target_link_libraries(FluidSimulation sfml-system sfml-window sfml-graphics sfml-audio sfml-network)

