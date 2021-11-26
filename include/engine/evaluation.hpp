#pragma once
#include "representation/position.hpp"
#include "representation/position.hpp"

struct PositionEval
{
    int white_material = 0;
    int black_material = 0;
};

struct PositionEval count_material(std::shared_ptr<Position> position);

int basic_material_for_piece(piece_t piece);

// negative is good for black, positive is good for white
int evaluate(std::shared_ptr<Position> position);
