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
    expected_position->advance_position(D2_SQ, D4_SQ, VOID_PIECE);

    std::shared_ptr<Position> fen_position =
        fen_to_position("rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 1");

    REQUIRE((*fen_position) == (*expected_position));
}

TEST_CASE("starting position is correctly read from fen string after a set of moves", "[fen_to_position]")
{
    auto expected_position = starting_position();
    expected_position->advance_position(D2_SQ, D4_SQ, VOID_PIECE);
    expected_position->advance_position(D7_SQ, D5_SQ, VOID_PIECE);
    expected_position->advance_position(C2_SQ, C4_SQ, VOID_PIECE);
    expected_position->advance_position(C7_SQ, C6_SQ, VOID_PIECE);
    expected_position->advance_position(G1_SQ, F3_SQ, VOID_PIECE);
    expected_position->advance_position(G8_SQ, F6_SQ, VOID_PIECE);

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

TEST_CASE("", "")
{

    // LASTLEFTOFF castling input provided by UI is encoded as e1c1. advance_position doesnt
    // properly treat this.

    // position startpos moves d2d4 g8f6 c2c4 g7g6 b1c3 d7d5 c1g5 f6e4 c3e4 c8f5 c4d5
    // c7c6 d5c6 h7h6 g5h6 a7a6 g1f3 d8c8 d1b3 b7b5 b3b5 c8c6 b5c6 e8d8 c6b6 d8d7 e4c5
    // d7c8 b6a6 c8c7 f3e5 h8h6 a6a5 c7d6 a5d8 b8d7 d8d7 f5d7 e5d7 e7e6 d7b6 d6c7 b6c4
    // h6h5 e1c1 f7f6 c1b1 c7d8 c5b7 d8e7 d1c1

    // r4b2/1N2k3/4ppp1/7r/2NP4/8/PP2PPPP/RK3B1R w  - 0 1
}