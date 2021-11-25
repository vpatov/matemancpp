cmake_minimum_required(VERSION 3.5)
project(matemancpp LANGUAGES CXX VERSION 0.1.0)



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
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -pg")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

include_directories( ${Boost_INCLUDE_DIRS} )

add_subdirectory (src) 
add_subdirectory (tests)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# enable_testing ()
# add_test (NAME MyTest COMMAND Test)


# add_executable(matemancpp ${MATEMAN_SOURCES})
# add_executable(test_runner ${TEST_SOURCES})


# find_path(BOOST_ALGORITHM_INCLUDE_DIRS "boost/algorithm/algorithm.hpp")
# target_include_directories(matemancpp PRIVATE ${BOOST_ALGORITHM_INCLUDE_DIRS})
# find_path(BOOST_FORMAT_INCLUDE_DIRS "boost/format.hpp")
# target_include_directories(matemancpp PRIVATE ${BOOST_FORMAT_INCLUDE_DIRS})











