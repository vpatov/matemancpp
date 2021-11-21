#pragma once
#include <ostream>
#include "representation/squares.hpp"
#include "representation/pieces.hpp"

// move key is bit-wise concatenation of
// 0x00 + start_square + end_square + promotion_piece
typedef uint32_t MoveKey;

struct Move
{
    square_t src_square;
    square_t dest_square;
    piece_t promotion_piece;

    friend std::ostream &operator<<(std::ostream &os, Move &move);
};

MoveKey generate_move_key(square_t src_square, square_t dest_square, piece_t promotion_piece);
Move unpack_move_key(MoveKey move_key);
