
set(SOURCES
process_pgn/pgn_game.cpp
process_pgn/read_pgn_data.cpp
process_pgn/pgn_position.cpp
cli.cpp
representation/position.cpp
representation/fen.cpp
representation/notation.cpp
options.cpp
move_generation.cpp
tablebase/move.cpp
tablebase/persistence.cpp
tablebase/tablebase.cpp
tablebase/zobrist.cpp
tablebase/move_edge.cpp
threadpool/threadpool.cpp
util.cpp
engine/engine.cpp
engine/evaluation.cpp
../include/cli.hpp
../include/engine/engine.hpp
../include/engine/evaluation.hpp
../include/representation/position.hpp
../include/representation/move.hpp
../include/representation/pieces.hpp
../include/representation/fen.hpp
../include/representation/color.hpp
../include/representation/notation.hpp
../include/options.hpp
../include/representation/offsets.hpp
../include/move_generation.hpp
../include/threadpool/threadpool.hpp
../include/representation/squares.hpp
../include/process_pgn/read_pgn_data.hpp
../include/process_pgn/pgn_game.hpp
../include/util.hpp
../include/tablebase/tablebase.hpp
../include/tablebase/move_edge.hpp
../include/tablebase/zobrist.hpp
# include/test/launcher.hpp
)

add_library (matemancpp_lib ${SOURCES})
add_executable (matemancpp main.cpp)
target_link_libraries(matemancpp PRIVATE Threads::Threads)
target_link_libraries(matemancpp PRIVATE spdlog::spdlog)
target_link_libraries (matemancpp PRIVATE matemancpp_lib)


include_directories(../include)
target_include_directories(matemancpp PRIVATE ../include)

add_compile_definitions(PROJECT_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/..")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ../build)



