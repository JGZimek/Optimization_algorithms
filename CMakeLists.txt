cmake_minimum_required(VERSION 3.10)

project(OptimizationAlgorithms)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(neh src/neh.cpp src/task_loader.cpp)
add_executable(sa src/sa.cpp src/task_loader.cpp)
add_executable(witi src/witi.cpp src/task_loader.cpp)
