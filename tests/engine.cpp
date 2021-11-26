#include "catch.hpp"
#include "representation/position.hpp"
#include "representation/fen.hpp"
#include "engine/engine.hpp"

TEST_CASE("position adjustment unroll yields same position", "[fen_to_position]")
{
    auto position = starting_position();
    auto adjustment = position->advance_position(E2_SQ, E4_SQ, VOID_PIECE);
    position->undo_adjustment(adjustment);
    REQUIRE((*position) == (*starting_position()));
}

TEST_CASE("multiple position adjustment unroll yields same position", "[fen_to_position]")
{
    PositionAdjustment adjustments[8];
    auto position = starting_position();
    int index = 0;

    adjustments[index++] = position->advance_position(E2_SQ, E4_SQ, VOID_PIECE);
    adjustments[index++] = position->advance_position(E7_SQ, E5_SQ, VOID_PIECE);
    adjustments[index++] = position->advance_position(G1_SQ, F3_SQ, VOID_PIECE);
    adjustments[index++] = position->advance_position(G8_SQ, F6_SQ, VOID_PIECE);
    adjustments[index++] = position->advance_position(F1_SQ, C4_SQ, VOID_PIECE);
    adjustments[index++] = position->advance_position(F8_SQ, C5_SQ, VOID_PIECE);
    adjustments[index++] = position->advance_position(E1_SQ, G1_SQ, VOID_PIECE);
    adjustments[index] = position->advance_position(E8_SQ, G8_SQ, VOID_PIECE);

    auto expected_position =
        fen_to_position("rnbq1rk1/pppp1ppp/5n2/2b1p3/2B1P3/5N2/PPPP1PPP/RNBQ1RK1 w - - 6 5");

    REQUIRE((*expected_position) == (*position));

    while (index >= 0)
    {
        position->undo_adjustment(adjustments[index--]);
    }

    REQUIRE((*position) == (*starting_position()));
}
