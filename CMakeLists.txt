cmake_minimum_required (VERSION 3.2)

project(oop_exercise_08)

set(CMAKE_CXX_STANDARD 17)

add_executable(oop_exercise_08 main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread")
