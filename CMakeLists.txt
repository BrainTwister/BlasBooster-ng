cmake_minimum_required(VERSION 3.1)
project(BlasBooster-ng VERSION 0.1 LANGUAGES CXX)

# All compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_subdirectory(tests)
