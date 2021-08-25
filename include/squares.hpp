#pragma once
#include <cstdint>

const int MAILBOX_LENGTH = 128;
typedef uint8_t square_t;

const square_t A1_SQ = 0;
const square_t B1_SQ = 1;
const square_t C1_SQ = 2;
const square_t D1_SQ = 3;
const square_t E1_SQ = 4;
const square_t F1_SQ = 5;
const square_t G1_SQ = 6;
const square_t H1_SQ = 7;
const square_t A2_SQ = 16;
const square_t B2_SQ = 17;
const square_t C2_SQ = 18;
const square_t D2_SQ = 19;
const square_t E2_SQ = 20;
const square_t F2_SQ = 21;
const square_t G2_SQ = 22;
const square_t H2_SQ = 23;
const square_t A3_SQ = 32;
const square_t B3_SQ = 33;
const square_t C3_SQ = 34;
const square_t D3_SQ = 35;
const square_t E3_SQ = 36;
const square_t F3_SQ = 37;
const square_t G3_SQ = 38;
const square_t H3_SQ = 39;
const square_t A4_SQ = 48;
const square_t B4_SQ = 49;
const square_t C4_SQ = 50;
const square_t D4_SQ = 51;
const square_t E4_SQ = 52;
const square_t F4_SQ = 53;
const square_t G4_SQ = 54;
const square_t H4_SQ = 55;
const square_t A5_SQ = 64;
const square_t B5_SQ = 65;
const square_t C5_SQ = 66;
const square_t D5_SQ = 67;
const square_t E5_SQ = 68;
const square_t F5_SQ = 69;
const square_t G5_SQ = 70;
const square_t H5_SQ = 71;
const square_t A6_SQ = 80;
const square_t B6_SQ = 81;
const square_t C6_SQ = 82;
const square_t D6_SQ = 83;
const square_t E6_SQ = 84;
const square_t F6_SQ = 85;
const square_t G6_SQ = 86;
const square_t H6_SQ = 87;
const square_t A7_SQ = 96;
const square_t B7_SQ = 97;
const square_t C7_SQ = 98;
const square_t D7_SQ = 99;
const square_t E7_SQ = 100;
const square_t F7_SQ = 101;
const square_t G7_SQ = 102;
const square_t H7_SQ = 103;
const square_t A8_SQ = 112;
const square_t B8_SQ = 113;
const square_t C8_SQ = 114;
const square_t D8_SQ = 115;
const square_t E8_SQ = 116;
const square_t F8_SQ = 117;
const square_t G8_SQ = 118;
const square_t H8_SQ = 119;

const square_t W_KING_SQUARE = E1_SQ;
const square_t B_KING_SQUARE = E8_SQ;

const square_t W_KING_SHORT_CASTLE_SQUARE = G1_SQ;
const square_t B_KING_SHORT_CASTLE_SQUARE = G8_SQ;
const square_t W_KING_LONG_CASTLE_SQUARE = C1_SQ;
const square_t B_KING_LONG_CASTLE_SQUARE = C8_SQ;

const square_t W_ROOK_SHORT_CASTLE_SQUARE = F1_SQ;
const square_t B_ROOK_SHORT_CASTLE_SQUARE = F8_SQ;
const square_t W_ROOK_LONG_CASTLE_SQUARE = D1_SQ;
const square_t B_ROOK_LONG_CASTLE_SQUARE = D8_SQ;

const square_t W_KING_ROOK_SQUARE = H1_SQ;
const square_t W_QUEEN_ROOK_SQUARE = A1_SQ;
const square_t B_KING_ROOK_SQUARE = H8_SQ;
const square_t B_QUEEN_ROOK_SQUARE = A8_SQ;

const square_t INVALID_SQUARE = 127;

inline bool is_invalid_square(square_t sq)
{
    return sq & 0x88;
}

inline bool is_valid_square(square_t sq)
{
    return !(sq & 0x88);
}
