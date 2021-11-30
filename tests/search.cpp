#include "catch.hpp"
#include "engine/search.hpp"
#include "representation/fen.hpp"
#include "representation/move.hpp"

TEST_CASE("minmax search returns expected move 01", "[minmax_search]")
{
    auto position = fen_to_position("r7/8/k7/3N4/8/PK5P/8/8 w - - 0 1");
    auto movekey = minmax_search(position, 4);
    auto expected_movekey = lan_to_movekey("d5c7");

    REQUIRE(movekey == expected_movekey);
}

TEST_CASE("minmax search returns expected move 02", "[minmax_search]")
{
    auto position = fen_to_position("k7/8/8/8/8/5r2/B7/K7 w - - 0 1");
    auto movekey = minmax_search(position, 4);
    auto expected_movekey = lan_to_movekey("a2d5");

    REQUIRE(movekey == expected_movekey);
}