#include "representation/move.hpp"
#include <sstream>

Move unpack_move_key(MoveKey move_key)
{
    Move move(move_key >> 16, (move_key & 0xff00) >> 8, move_key & 0xff);
    return move;
}

std::string movekey_to_lan(MoveKey move_key)
{
    std::stringstream ss;
    auto move = unpack_move_key(move_key);
    ss << index_to_an_square(move.m_src_square) << index_to_an_square(move.m_dst_square);
    if (move.m_promotion_piece)
    {
        ss << "=" << piece_to_char(move.m_promotion_piece);
    }
    return ss.str();
}

/* 
  This function calculates and returns the move key, which is a concatenation 
  of the source square, destination square, and promotion piece (complete 
  information necessary to understand a move). For castling moves, even though
  two pieces move, the src_square and dest_square pertain only to the king.
*/
MoveKey generate_move_key(square_t src_square, square_t dest_square, piece_t promotion_piece)
{
    // move key is bit-wise concatenation of
    // (empty/reserved) + start_square + end_square + promotion_piece
    // 8 bits             8 bits         8 bits       8 bits
    return ((uint32_t)src_square << 16) + ((uint32_t)dest_square << 8) + promotion_piece;
}

std::ostream &operator<<(std::ostream &os, Move &move)
{
    os
        << "src_square: " << move.m_src_square << std::endl
        << "dest_square: " << move.m_dst_square << std::endl
        << "promotion_piece: " << move.m_promotion_piece << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, MoveEdge &move_edge)
{
    os
        << "dest_hash: " << move_edge.m_dest_hash << std::endl
        << "pgn_move: " << move_edge.m_pgn_move << std::endl
        << "times_played: " << move_edge.m_times_played << std::endl
        << std::endl;
    return os;
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