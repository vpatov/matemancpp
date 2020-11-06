cmake_minimum_required(VERSION 3.0.0)
project(matemancpp VERSION 0.1.0)

set(SOURCES
    src/main.cpp
    src/cli.cpp
    include/cli.hpp
)




add_executable(matemancpp src/main.cpp)


find_path(BOOST_ALGORITHM_INCLUDE_DIRS "boost/algorithm/algorithm.hpp")
target_include_directories(matemancpp PRIVATE ${BOOST_ALGORITHM_INCLUDE_DIRS})

target_include_directories(matemancpp PRIVATE ${PROJECT_SOURCE_DIR}/include)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(matemancpp PRIVATE Threads::Threads)
