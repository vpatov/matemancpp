#include "catch.hpp"
#include "representation/position.hpp"
#include "representation/pieces.hpp"
#include "representation/fen.hpp"
#include "engine/engine.hpp"
#include "move_generation.hpp"
#include <iostream>
#include <set>

TEST_CASE("move generation works correctly for starting position", "[move_generation]")
{
    auto position = starting_position();
    auto move_list = get_all_moves(position);
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

    position->advance_position(E2_SQ, E4_SQ, 0);
    move_list = get_all_moves(position);

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

TEST_CASE("doesn't generate illegal moves 01", "[move_generation]")
{
    auto position =
        fen_to_position("rnbqkbnr/ppp2Qpp/8/3pp3/4P3/8/PPPP1PPP/RNB1KBNR b KQkq - 0 3");

    auto moves = get_all_moves(position);
    REQUIRE(moves.size() == 1);
    REQUIRE(moves.at(0) == m(E8_SQ, F7_SQ));
}

TEST_CASE("doesn't generate illegal moves 02", "[move_generation]")
{
    auto position =
        fen_to_position("rnbqkbnr/ppp2Bpp/8/3pp3/4P3/8/PPPP1PPP/RNB1KBNR b KQkq - 0 3");

    auto moves = get_all_moves(position);
    REQUIRE(moves.size() == 3);
}

TEST_CASE("doesn't generate illegal moves 03", "[move_generation]")
{
    auto position =
        fen_to_position("rnbqkbnr/ppp2Ppp/8/3pp3/4P3/8/PPPP1PPP/RNB1KBNR b KQkq - 0 3");

    auto moves = get_all_moves(position);
    REQUIRE(moves.size() == 3);
}

template <typename T>
bool contains(std::vector<T> *arg, T target)
{
    return std::find(arg->begin(), arg->end(), target) != arg->end();
}

TEST_CASE("doesn't generate illegal moves 04", "[move_generation]")
{
    auto position =
        fen_to_position("2b4k/pppprp1p/6p1/8/1b5q/1P6/P1PPBPPP/RrQ1KNrR w KQ - 0 1");
    auto moves = get_all_moves(position);

    REQUIRE(contains(&moves, m(A1_SQ, B1_SQ)));
    REQUIRE(contains(&moves, m(C1_SQ, B1_SQ)));
    REQUIRE(contains(&moves, m(C1_SQ, D1_SQ)));
    REQUIRE(contains(&moves, m(E1_SQ, D1_SQ)));
    REQUIRE(contains(&moves, m(H1_SQ, G1_SQ)));

    REQUIRE(contains(&moves, m(A2_SQ, A4_SQ)));
    REQUIRE(contains(&moves, m(A2_SQ, A3_SQ)));
    REQUIRE(contains(&moves, m(C2_SQ, C3_SQ)));
    REQUIRE(contains(&moves, m(C2_SQ, C4_SQ)));
    REQUIRE(contains(&moves, m(G2_SQ, G3_SQ)));
    REQUIRE(contains(&moves, m(G2_SQ, G4_SQ)));
    REQUIRE(contains(&moves, m(H2_SQ, H3_SQ)));

    REQUIRE(moves.size() == 12);
}

TEST_CASE("make illegal move, and both kings are in check", "[move_generation]")
{
    auto position =
        fen_to_position("2b4k/pppprp1p/6p1/8/1b5q/1P6/P1PPBPPP/RrQ1KNrR w KQ - 0 1");
    position->advance_position(m(C1_SQ, B2_SQ));

    REQUIRE(position->is_king_in_check(true));
    REQUIRE(position->is_king_in_check(false));
}
