#include "catch.hpp"
#include "representation/position.hpp"
#include "representation/fen.hpp"

const std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

TEST_CASE("starting position is correctly read from fen string", "[fen_to_position]")
{
    std::shared_ptr<Position> position =
        fen_to_position(STARTING_FEN);

    REQUIRE((*position) == (*starting_position()));
}

TEST_CASE("starting position is correctly read from fen string after d4", "[fen_to_position]")
{
    auto expected_position = starting_position();
    expected_position->advance_position2(D2_SQ, D4_SQ, VOID_PIECE);

    std::shared_ptr<Position> fen_position =
        fen_to_position("rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 1");

    REQUIRE((*fen_position) == (*expected_position));
}

TEST_CASE("starting position is correctly read from fen string after a set of moves", "[fen_to_position]")
{
    auto expected_position = starting_position();
    expected_position->advance_position2(D2_SQ, D4_SQ, VOID_PIECE);
    expected_position->advance_position2(D7_SQ, D5_SQ, VOID_PIECE);
    expected_position->advance_position2(C2_SQ, C4_SQ, VOID_PIECE);
    expected_position->advance_position2(C7_SQ, C6_SQ, VOID_PIECE);
    expected_position->advance_position2(G1_SQ, F3_SQ, VOID_PIECE);
    expected_position->advance_position2(G8_SQ, F6_SQ, VOID_PIECE);

    std::shared_ptr<Position> fen_position =
        fen_to_position("rnbqkb1r/pp2pppp/2p2n2/3p4/2PP4/5N2/PP2PPPP/RNBQKB1R w KQkq - 2 4");

    REQUIRE((*fen_position) == (*expected_position));
}

TEST_CASE("position_to_fen returns expected FEN ", "[position_to_fen]")
{
    auto position = starting_position();
    std::string fen = position_to_fen(position);

    REQUIRE(STARTING_FEN == fen);
}

TEST_CASE("pos->fen->pos returns original string", "[position_to_fen, fen_to_position]")
{
    auto start_position = starting_position();
    std::string fen = position_to_fen(start_position);
    auto fen_position = fen_to_position(fen);

    REQUIRE((*fen_position) == (*start_position));
}

TEST_CASE("fen->pos->fen returns original string", "[position_to_fen, fen_to_position]")
{
    auto position = fen_to_position(STARTING_FEN);
    std::string fen = position_to_fen(position);

    REQUIRE(fen == STARTING_FEN);
}