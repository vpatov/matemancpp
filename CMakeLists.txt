cmake_minimum_required(VERSION 3.0.0)
project(matemancpp VERSION 0.1.0)

add_executable(matemancpp src/main.cpp)

find_path(BOOST_ALGORITHM_INCLUDE_DIRS "boost/algorithm/algorithm.hpp")
target_include_directories(matemancpp PRIVATE ${BOOST_ALGORITHM_INCLUDE_DIRS})

