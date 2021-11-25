#include "catch.hpp"
#include "representation/position.hpp"
#include "representation/fen.hpp"

TEST_CASE("starting position is correctly read from fen string", "[fen_to_position]")
{
    // Example FEN string (starting position):
    // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    std::shared_ptr<Position> position = fen_to_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    REQUIRE((*position) == (*starting_position()));
}

TEST_CASE("starting position is correctly read from fen string after d4", "[fen_to_position]")
{
    // Example FEN string (starting position):
    // rnbqkbnr/ppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    std::shared_ptr<Position> fen_position = fen_to_position("rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 1");
    auto expected_position = starting_position();
    expected_position->advance_position2(D2_SQ, D4_SQ, VOID_PIECE);
    REQUIRE((*fen_position) == (*expected_position));
}