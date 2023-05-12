cmake_minimum_required(VERSION 3.1)
project(BlasBooster-ng VERSION 0.1 LANGUAGES CXX)

# All compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)

enable_testing()
add_subdirectory(tests)
