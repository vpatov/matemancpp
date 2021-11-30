#pragma once
#include <ostream>
#include "representation/squares.hpp"
#include "representation/pieces.hpp"
#include "representation/notation.hpp"
#include <sstream>

// move key is bit-wise concatenation of
// 0x00 + start_square + end_square + promotion_piece
typedef uint32_t MoveKey;
const MoveKey VOID_MOVE = 0;

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
    Move(MoveKey movekey)
    {
        m_src_square = (movekey >> 16) & 0xff;
        m_dst_square = (movekey >> 8) & 0xff;
        m_promotion_piece = movekey & 0xff;
    }

    friend std::ostream &operator<<(std::ostream &os, Move &move);

    MoveKey to_move_key()
    {
        return ((uint32_t)m_src_square << 16) + ((uint32_t)m_dst_square << 8) + m_promotion_piece;
    }

    bool operator==(const Move &rhs) const
    {
        return m_src_square == rhs.m_src_square &&
               m_dst_square == rhs.m_dst_square &&
               m_promotion_piece == rhs.m_promotion_piece;
    }
};

std::string generate_long_algebraic_notation(MoveKey move_key);
MoveKey generate_move_key(square_t src_square, square_t dest_square, piece_t promotion_piece);
Move unpack_move_key(MoveKey move_key);
std::string movekey_to_lan(MoveKey move_key);

inline MoveKey pack_move_key(square_t src_square, square_t dst_square, piece_t promotion_piece)
{
    return ((uint32_t)src_square << 16) + ((uint32_t)dst_square << 8) + promotion_piece;
}

inline MoveKey pack_move_key(square_t src_square, square_t dst_square)
{
    return pack_move_key(src_square, dst_square, VOID_PIECE);
}

inline std::string movekey_to_lan(MoveKey move_key)
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

inline Move lan_to_move(std::string move_str)
{
    square_t src_square = an_square_to_index(move_str.substr(0, 2));
    square_t dst_square = an_square_to_index(move_str.substr(2, 4));
    piece_t promotion_piece = VOID_PIECE;

    // piece should always be uppercase because pieces are uppercase in PGN.

    // this code should probably be there for the "position startpos moves ...." path too
    if (move_str.size() >= 5)
    {
        if (move_str.at(4) == '=')
        {
            promotion_piece = char_to_piece(move_str.at(5));
        }
        else if (is_possible_promotion_piece(move_str.at(4)))
        {
            promotion_piece = char_to_piece(move_str.at(4));
        }
        else
        {
            assert(false);
        }
        // promotion piece is always white, and then appropriate color is set
        // when the piece is placed on the board.
        promotion_piece &= PIECE_MASK;
    }
    return Move(src_square, dst_square, promotion_piece);
}

inline MoveKey m(square_t sq1, square_t sq2)
{
    return pack_move_key(sq1, sq2);
}

inline MoveKey lan_to_movekey(std::string move_str)
{
    return lan_to_move(move_str).to_move_key();
}