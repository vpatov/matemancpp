#include "catch.hpp"
#include "representation/position.hpp"
#include "representation/pieces.hpp"
#include "representation/fen.hpp"
#include "engine/engine.hpp"
#include "move_generation.hpp"
#include <iostream>
#include <set>

MoveKey m(square_t sq1, square_t sq2)
{
    return pack_move_key(sq1, sq2);
}

TEST_CASE("move generation works correctly for starting position", "[move_generation]")
{
    Engine engine;
    engine.m_current_position = starting_position();
    auto move_list = engine.get_all_moves();
    std::set<MoveKey> move_set(move_list.begin(), move_list.end());

    REQUIRE(move_list.size() == move_set.size());

    std::set<MoveKey> expected_moves = {
        m(A2_SQ, A3_SQ),
        m(A2_SQ, A4_SQ),
        m(B2_SQ, B3_SQ),
        m(B2_SQ, B4_SQ),
        m(C2_SQ, C3_SQ),
        m(C2_SQ, C4_SQ),
        m(D2_SQ, D3_SQ),
        m(D2_SQ, D4_SQ),
        m(E2_SQ, E3_SQ),
        m(E2_SQ, E4_SQ),
        m(F2_SQ, F3_SQ),
        m(F2_SQ, F4_SQ),
        m(G2_SQ, G3_SQ),
        m(G2_SQ, G4_SQ),
        m(H2_SQ, H3_SQ),
        m(H2_SQ, H4_SQ),
        m(G1_SQ, F3_SQ),
        m(G1_SQ, H3_SQ),
        m(B1_SQ, A3_SQ),
        m(B1_SQ, C3_SQ)};

    REQUIRE(expected_moves.size() == move_set.size());
    for (auto it = expected_moves.begin(); it != expected_moves.end(); it++)
    {
        REQUIRE(move_set.find(*it) != move_set.end());
    }

    engine.m_current_position->advance_position2(E2_SQ, E4_SQ, 0);
    move_list = engine.get_all_moves();
    move_set = std::set<MoveKey>(move_list.begin(), move_list.end());
    REQUIRE(move_list.size() == move_set.size());

    expected_moves = {
        m(A7_SQ, A6_SQ),
        m(A7_SQ, A5_SQ),
        m(B7_SQ, B6_SQ),
        m(B7_SQ, B5_SQ),
        m(C7_SQ, C6_SQ),
        m(C7_SQ, C5_SQ),
        m(D7_SQ, D6_SQ),
        m(D7_SQ, D5_SQ),
        m(E7_SQ, E6_SQ),
        m(E7_SQ, E5_SQ),
        m(F7_SQ, F6_SQ),
        m(F7_SQ, F5_SQ),
        m(G7_SQ, G6_SQ),
        m(G7_SQ, G5_SQ),
        m(H7_SQ, H6_SQ),
        m(H7_SQ, H5_SQ),
        m(G8_SQ, F6_SQ),
        m(G8_SQ, H6_SQ),
        m(B8_SQ, A6_SQ),
        m(B8_SQ, C6_SQ)};

    REQUIRE(expected_moves.size() == move_set.size());
    for (auto it = expected_moves.begin(); it != expected_moves.end(); it++)
    {
        REQUIRE(move_set.find(*it) != move_set.end());
    }
}

TEST_CASE("doesn't generate illegal moves", "[move_generation]")
{
    Engine engine;
    engine.m_current_position =
        fen_to_position("rnbqkbnr/ppp2Qpp/8/3pp3/4P3/8/PPPP1PPP/RNB1KBNR b KQkq - 0 3");

    auto moves = engine.get_all_moves();
    REQUIRE(moves.size() == 1);
    REQUIRE(moves.at(0) == m(E8_SQ, F7_SQ));
}
