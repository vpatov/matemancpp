#include "catch.hpp"
#include "representation/position.hpp"
#include "representation/fen.hpp"
#include "engine/engine.hpp"
#include "engine/evaluation.hpp"

TEST_CASE("material evaluation for starting position", "[basic_materia;]")
{
    auto position = starting_position();
    auto material_eval = count_material(position);

    REQUIRE(material_eval.white_material == 39);
    REQUIRE(material_eval.black_material == 39);
}

TEST_CASE("material evaluation after captures", "[basic_materia;]")
{
    auto position = starting_position();
    position->advance_position(E2_SQ, E4_SQ);
    position->advance_position(E7_SQ, E5_SQ);
    position->advance_position(D1_SQ, H5_SQ);
    position->advance_position(A7_SQ, A6_SQ);
    position->advance_position(H5_SQ, F7_SQ);
    position->advance_position(E8_SQ, F7_SQ);
    auto material_eval = count_material(position);

    REQUIRE(material_eval.white_material == 30);
    REQUIRE(material_eval.black_material == 38);
}