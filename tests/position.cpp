#include "catch.hpp"
#include "representation/position.hpp"
#include "representation/fen.hpp"

TEST_CASE("position copy makes different position", "[fen_to_position]")
{
    auto position = starting_position();
    auto new_position = std::make_shared<Position>(*position);
    REQUIRE((*position) == (*new_position));
    REQUIRE(position != new_position);

    new_position->advance_position(E2_SQ, E4_SQ);

    REQUIRE((*position) != (*new_position));
}

TEST_CASE("is_king_in_check returns correct answer", "is_king_in_check")
{
    auto position = starting_position();
    position->advance_position(m(E2_SQ, E4_SQ));
    position->advance_position(m(E7_SQ, E5_SQ));
    position->advance_position(m(D1_SQ, H5_SQ));
    position->advance_position(m(F7_SQ, F6_SQ));

    REQUIRE(position->is_king_in_check(false));
    REQUIRE(!position->is_king_in_check(true));
}
