#include "tablebase/movekey.hpp"

Move unpack_move_key(uint32_t move_key)
{
    Move move;
    move.src_square = move_key >> 16;
    move.dest_square = (move_key & 0xff00) >> 8;
    move.promotion_piece = move_key & 0xff;
    return move;
}

std::ostream &operator<<(std::ostream &os, Move &move)
{
    os
        << "src_square: " << move.src_square << std::endl
        << "dest_square: " << move.dest_square << std::endl
        << "promotion_piece: " << move.promotion_piece << std::endl;
    return os;
}