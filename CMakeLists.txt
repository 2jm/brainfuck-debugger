cmake_minimum_required(VERSION 3.3)
project(assa)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Werror")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(SOURCE_FILES assa/assa.c assa/interpreter.c assa/interpreter.h
        assa/debugFunctions.c assa/debugFunctions.h)

# bonus only
#set(SOURCE_FILES assa/bio.c)

# test interpreter only
#set(SOURCE_FILES assa/interpretertest.c assa/interpreter.c assa/interpreter.h)

add_executable(assa ${SOURCE_FILES})