#pragma once

#include "tablebase/zobrist.hpp"
#include <cstring>

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

    bool operator==(const MoveEdge &rhs) const
    {
        return m_dest_hash == rhs.m_dest_hash &&
               m_times_played == rhs.m_times_played &&
               strcmp(m_pgn_move, rhs.m_pgn_move) == 0;
    }
    bool operator!=(const MoveEdge &rhs) const
    {
        return !((*this) == rhs);
    }
};

bool compare_key_move_pair(std::pair<MoveKey, MoveEdge> p1, std::pair<MoveKey, MoveEdge> p2);
std::ostream &operator<<(std::ostream &os, MoveEdge &move_edge);

bool compare_key_move_pair(std::pair<MoveKey, MoveEdge> p1, std::pair<MoveKey, MoveEdge> p2);
