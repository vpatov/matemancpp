#pragma once
#include <ostream>
#include "representation/squares.hpp"
#include "representation/pieces.hpp"
#include "tablebase/zobrist.hpp"

// move key is bit-wise concatenation of
// 0x00 + start_square + end_square + promotion_piece
typedef uint32_t MoveKey;

// try using move in move generation because it's simpler to interface with
// later, if performance improvements are necessary, consider using 16-bit movekey
struct Move
{
    square_t m_src_square;
    square_t m_dst_square;
    piece_t m_promotion_piece;

    Move(square_t src_square, square_t dst_square, piece_t promotion_piece)
        : m_src_square(src_square), m_dst_square(dst_square), m_promotion_piece(promotion_piece) {}
    Move(square_t src_square, square_t dst_square)
        : m_src_square(src_square), m_dst_square(dst_square), m_promotion_piece(0) {}

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
std::string movekey_to_lan(MoveKey move_key);

// since there are only 4 promotion candidates, we can use 2 bits to represent them.
// promotion_piece (2 bits) - dst_square (7 bits) - src_square (7 bits)
// color of promotion piece can be inferred from color of src_square piece
typedef uint16_t MoveKey_16;

inline MoveKey_16 pack_move_key_16(square_t src_square, square_t dst_square, piece_t promotion_piece)
{
    uint8_t promotion_bits = 0;
    switch (promotion_piece & PIECE_MASK)
    {
    case VOID_PIECE:
        promotion_bits = 0;
        break;
    case ROOK:
        promotion_bits = 1;
        break;
    case KNIGHT:
        promotion_bits = 2;
        break;
    case BISHOP:
        promotion_bits = 3;
        break;
    case QUEEN:
        promotion_bits = 4;
        break;
    default:
        __builtin_unreachable();
    }
    return (uint16_t)src_square | (dst_square << 7) | promotion_piece << 14;
    //    return ((uint16_t)src_square << 9) | ()
}

inline Move unpack_move_key_16(MoveKey_16 movekey)
{
    piece_t promotion_piece = 0;
    switch (movekey >> 14)
    {
    case VOID_PIECE:
        promotion_piece = 0;
        break;
    case 1:
        promotion_piece = ROOK;
        break;
    case 2:
        promotion_piece = KNIGHT;
        break;
    case 3:
        promotion_piece = BISHOP;
        break;
    case 4:
        promotion_piece = QUEEN;
        break;
    default:
        __builtin_unreachable();
    }
    return Move(movekey & 0x7f, (movekey >> 7) & 0x7f, promotion_piece);
}

inline MoveKey pack_move_key(square_t src_square, square_t dst_square)
{
    return ((uint32_t)src_square << 16) + ((uint32_t)dst_square << 8);
}

inline MoveKey pack_move_key(square_t src_square, square_t dst_square, piece_t promotion_piece)
{
    return ((uint32_t)src_square << 16) + ((uint32_t)dst_square << 8) + promotion_piece;
}