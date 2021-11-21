cmake_minimum_required(VERSION 3.0.0)
project(matemancpp VERSION 0.1.0)

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
set(BOOST_ROOT /usr/local/boost_1_76_0)

find_package(Boost 1.76.0 COMPONENTS algorithm format) 

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()

#find_package( Boost REQUIRED COMPONENTS algorithm format )
include_directories( ${Boost_INCLUDE_DIRS} )

set(SOURCES
    src/process_pgn/pgn_game.cpp
    src/process_pgn/read_pgn_data.cpp
    src/process_pgn/pgn_position.cpp
    src/main.cpp
    src/cli.cpp
    src/representation/position.cpp
    src/representation/fen.cpp
    src/options.cpp
    src/move_generation.cpp
    src/tablebase/tablebase.cpp
    src/tablebase/zobrist.cpp
    src/threadpool/threadpool.cpp
    src/util.cpp
    src/engine/engine.cpp
    include/cli.hpp
    include/engine/engine.hpp
    include/representation/position.hpp
    include/representation/pieces.hpp
    include/representation/fen.hpp
    include/options.hpp
    include/move_generation.hpp
    include/threadpool/threadpool.hpp
    include/representation/squares.hpp
    include/process_pgn/read_pgn_data.hpp
    include/process_pgn/pgn_game.hpp
    include/util.hpp
    include/tablebase/tablebase.hpp
    include/tablebase/persistence.hpp
    include/tablebase/zobrist.hpp
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
target_link_libraries(matemancpp PRIVATE spdlog::spdlog)


add_compile_definitions(PROJECT_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")



