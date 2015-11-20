cmake_minimum_required(VERSION 3.3)
project(assa)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Werror")

set(SOURCE_FILES assa.c)
add_executable(assa ${SOURCE_FILES})