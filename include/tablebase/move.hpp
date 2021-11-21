#pragma once
#include <ostream>
#include "representation/squares.hpp"
#include "representation/pieces.hpp"
#include "tablebase/zobrist.hpp"

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

struct MoveEdge
{
    z_hash_t m_dest_hash;
    uint32_t m_times_played;

    char m_pgn_move[8];

    friend std::ostream &operator<<(std::ostream &os, MoveEdge &move_edge);

    MoveEdge(z_hash_t dest_hash, std::string pgn_move)
    {
        m_dest_hash = dest_hash;
        assert(pgn_move.size() < 8);
        strncpy(m_pgn_move, pgn_move.c_str(), 8);

        m_times_played = 1;
    };

    MoveEdge(z_hash_t dest_hash, std::string pgn_move, uint32_t times_played)
    {
        m_dest_hash = dest_hash;
        assert(pgn_move.size() < 8);
        strncpy(m_pgn_move, pgn_move.c_str(), 8);

        m_times_played = times_played;
    };

    MoveEdge() {}

    MoveEdge(const MoveEdge &other)
    {
        m_dest_hash = other.m_dest_hash;
        m_times_played = other.m_times_played;
        strncpy(m_pgn_move, other.m_pgn_move, 8);
    }

    MoveEdge(MoveEdge &&other)
    {
        m_dest_hash = other.m_dest_hash;
        m_times_played = other.m_times_played;
        strncpy(m_pgn_move, other.m_pgn_move, 8);
    }
};

bool compare_key_move_pair(std::pair<MoveKey, MoveEdge> p1, std::pair<MoveKey, MoveEdge> p2);
std::ostream &operator<<(std::ostream &os, MoveEdge &move_edge);

std::string generate_long_algebraic_notation(MoveKey move_key);
MoveKey generate_move_key(square_t src_square, square_t dest_square, piece_t promotion_piece);
Move unpack_move_key(MoveKey move_key);
