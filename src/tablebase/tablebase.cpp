#include "tablebase/tablebase.hpp"
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

// | 00 | start_square | end_square | promotion_piece

std::string generate_long_algebraic_notation(MoveKey move_key)
{
    std::stringstream ss;
    ss << index_to_an_square((move_key >> 16) & 0xff);
    ss << index_to_an_square((move_key >> 8) & 0xff);
    piece_t promotion_piece = move_key & 0xff;

    if (promotion_piece)
    {
        ss << "=" << piece_to_char(promotion_piece);
    }
    return ss.str();
}

bool compare_key_move_pair(std::pair<MoveKey, MoveEdge> p1, std::pair<MoveKey, MoveEdge> p2)
{
    if (p1.second.m_times_played < p2.second.m_times_played)
    {
        return true;
    }
    else if (p1.second.m_times_played == p2.second.m_times_played)
    {
        return p1.second.m_dest_hash < p2.second.m_dest_hash;
    }
    else
    {
        return false;
    }
}

std::ostream &operator<<(std::ostream &os, MoveEdge &move_edge)
{
    // Move move = unpack_move_key(move_edge.m_move_key);
    os
        << "dest_hash: " << move_edge.m_dest_hash << std::endl
        << "pgn_move: " << move_edge.m_pgn_move << std::endl
        << "times_played: " << move_edge.m_times_played << std::endl
        // << "move_key: " << move_edge.m_move_key << std::endl
        << std::endl;

    return os;
}
