cmake_minimum_required(VERSION 3.10)
project(
    tkom_SIunits
    DESCRIPTION "Interpreter for programming language with SI units support"
    LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

#TODO include(CTest)
#TODO find_package(GTest)

set(BUILD_TESTING TRUE)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)

include_directories(src)

add_subdirectory(src)
add_subdirectory(src/source)
