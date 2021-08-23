#include "util.hpp"

/* 
  This function calculates and returns the move key, which is a concatenation 
  of the source square, destination square, and promotion piece (complete 
  information necessary to understand a move). For castling moves, even though
  two pieces move, the src_square and dest_square pertain only to the king.
*/
uint32_t generate_move_key(uint8_t src_square, uint8_t dest_square, uint8_t promotion_piece)
{
    // move key is bit-wise concatenation of
    // (empty/reserved) + start_square + end_square + promotion_piece
    // 8 bits             8 bits         8 bits       8 bits
    return (src_square << 16) + (dest_square << 8) + promotion_piece;
}

Move unpack_move_key(uint32_t move_key)
{
    Move move;
    move.src_square = move_key >> 16;
    move.dest_square = (move_key & 0xff00) >> 8;
    move.promotion_piece = move_key & 0xff;
    return move;
}

char getc(int i, std::smatch &matches)
{
    if (matches[i].length())
    {
        return matches[i].str().at(0);
    }
    return 0;
}