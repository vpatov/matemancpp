#include "tablebase.hpp"
#include "util.hpp"

bool compare_move_edge(MoveEdge move_edge1, MoveEdge move_edge2)
{
    return move_edge1.m_times_played < move_edge2.m_times_played;
}

std::ostream &operator<<(std::ostream &os, MoveEdge &move_edge)
{
    Move move = unpack_move_key(move_edge.m_move_key);
    os
        << "dest_hash: " << move_edge.m_dest_hash << std::endl
        << "pgn_move: " << move_edge.m_pgn_move << std::endl
        << "times_played: " << move_edge.m_times_played << std::endl
        << "move_key: " << move_edge.m_move_key << std::endl
        << std::endl;

    return os;
}
