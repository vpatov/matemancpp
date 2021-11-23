#pragma once

#include <cstdint>
#include <string>

typedef uint8_t piece_t;
typedef uint8_t piece_mask_t;

const piece_t VOID_PIECE = 0;

const char PAWN_CHAR = 'P';
const char ROOK_CHAR = 'R';
const char KNIGHT_CHAR = 'N';
const char BISHOP_CHAR = 'B';
const char QUEEN_CHAR = 'Q';
const char KING_CHAR = 'K';

const piece_t PAWN = 1;
const piece_t ROOK = 2;
const piece_t KNIGHT = 3;
const piece_t BISHOP = 4;
const piece_t QUEEN = 5;
const piece_t KING = 6;

const piece_t W_PAWN = PAWN;
const piece_t W_ROOK = ROOK;
const piece_t W_KNIGHT = KNIGHT;
const piece_t W_BISHOP = BISHOP;
const piece_t W_QUEEN = QUEEN;
const piece_t W_KING = KING;

const piece_mask_t BLACK_PIECE_MASK = 0x10;
const piece_mask_t PIECE_MASK = 0x7;

const piece_t B_PAWN = (PAWN | BLACK_PIECE_MASK);
const piece_t B_ROOK = (ROOK | BLACK_PIECE_MASK);
const piece_t B_KNIGHT(KNIGHT | BLACK_PIECE_MASK);
const piece_t B_BISHOP = (BISHOP | BLACK_PIECE_MASK);
const piece_t B_QUEEN = (QUEEN | BLACK_PIECE_MASK);
const piece_t B_KING = (KING | BLACK_PIECE_MASK);

const piece_t MAX_PIECE = B_KING;

const char PIECE_CHAR_MAP[24] = {'-', PAWN_CHAR, ROOK_CHAR, KNIGHT_CHAR, BISHOP_CHAR, QUEEN_CHAR, KING_CHAR, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, PAWN_CHAR + 32, ROOK_CHAR + 32, KNIGHT_CHAR + 32, BISHOP_CHAR + 32, QUEEN_CHAR + 32, KING_CHAR + 32, 0};

const std::string UNICODE_PIECE_CHAR_MAP[24] = {"-", "♙", "♖", "♘", "♗", "♕", "♔", " ",
                                                " ", " ", " ", " ", " ", " ", " ", " ",
                                                " ", "♟︎", "♜", "♞", "♝", "♛", "♚", " "};
inline bool is_empty(piece_t piece)
{
    return piece == VOID_PIECE;
}
inline bool is_piece(piece_t piece)
{
    return piece & PIECE_MASK;
}

inline bool is_black_piece(piece_t piece)
{
    return piece & BLACK_PIECE_MASK;
}

inline bool is_white_piece(piece_t piece)
{
    return piece && !(piece & BLACK_PIECE_MASK);
}