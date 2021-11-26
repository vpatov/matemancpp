find_package (Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
include_directories (${MATEMANCPP_SOURCE_DIR}/src
                     ${Boost_INCLUDE_DIRS}
                     )
add_definitions (-DBOOST_TEST_DYN_LINK)

set(SOURCES

    test.cpp
    fen.cpp
    catch.hpp
    move_generation.cpp
    engine.cpp
    evaluation.cpp
)

add_executable (Test ${SOURCES})
target_link_libraries (Test
                       matemancpp_lib
                       ${Boost_FILESYSTEM_LIBRARY}
                       ${Boost_SYSTEM_LIBRARY}
                       ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                       )

include_directories(../include)
add_compile_definitions(PROJECT_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")




