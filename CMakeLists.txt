cmake_minimum_required(VERSION 3.0.0)
project(matemancpp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
set(BOOST_ROOT /usr/local/boost_1_76_0)

find_package(Boost 1.76.0 COMPONENTS algorithm format) 


#find_package( Boost REQUIRED COMPONENTS algorithm format )
include_directories( ${Boost_INCLUDE_DIRS} )

set(SOURCES
    src/main.cpp
    src/cli.cpp
    src/position.cpp
    src/pgn_position.cpp
    src/fen.cpp
    src/options.cpp
    src/move_generation.cpp
    src/zobrist.cpp
    src/pgn_game.cpp
    src/read_pgn_data.cpp
    src/threadpool.cpp
    src/tablebase.cpp
    src/util.cpp
    include/cli.hpp
    include/position.hpp
    include/pieces.hpp
    include/fen.hpp
    include/options.hpp
    include/move_generation.hpp
    include/threadpool.hpp
    include/zobrist.hpp
    include/squares.hpp
    include/read_pgn_data.hpp
    include/tablebase.hpp
    include/master_tablebase.hpp
    include/util.hpp
)



add_executable(matemancpp ${SOURCES})

# find_path(BOOST_ALGORITHM_INCLUDE_DIRS "boost/algorithm/algorithm.hpp")
# target_include_directories(matemancpp PRIVATE ${BOOST_ALGORITHM_INCLUDE_DIRS})
# find_path(BOOST_FORMAT_INCLUDE_DIRS "boost/format.hpp")
# target_include_directories(matemancpp PRIVATE ${BOOST_FORMAT_INCLUDE_DIRS})

target_include_directories(matemancpp PRIVATE ${PROJECT_SOURCE_DIR}/include)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(matemancpp PRIVATE Threads::Threads)


