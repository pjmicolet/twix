cmake_minimum_required(VERSION 4.0)
set (CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Twix)

SET( CMAKE_CXX_FLAGS "-Wall -Wpedantic -Werror")

add_subdirectory(src)
