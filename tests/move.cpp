#include "catch.hpp"
#include "representation/fen.hpp"
#include "representation/move.hpp"

TEST_CASE("movekey -> move -> movekey yields original value 01")
{
    auto movekey = lan_to_movekey("e2e4");
    auto move = Move(movekey);

    REQUIRE(movekey == move.to_move_key());
}

TEST_CASE("movekey -> move -> movekey yields original value 02")
{
    auto movekey = lan_to_movekey("e7e8=Q");
    auto move = Move(movekey);

    REQUIRE(movekey == move.to_move_key());
}

TEST_CASE("movekey -> move -> movekey yields original value 03")
{
    auto movekey = lan_to_movekey("e7e8Q");
    auto move = Move(movekey);

    REQUIRE(movekey == move.to_move_key());
}

TEST_CASE("move -> movekey -> move yields original value 01")
{
    auto move = lan_to_move("d7d2");
    auto movekey = move.to_move_key();
    auto expected_move = Move(movekey);

    REQUIRE(move == expected_move);
}

TEST_CASE("move -> movekey -> move yields original value 02")
{
    auto move = lan_to_move("d7c8=n");
    auto movekey = move.to_move_key();
    auto expected_move = Move(movekey);

    REQUIRE(move == expected_move);
}