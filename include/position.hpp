#ifndef __POSITION_H__
#define __POSITION_H__

#include <cstdint>
#include <memory>
#include <string>

/**
  Mailbox

112 113 114 115 116 117 118 119 |   120 121 122 123 124 125 126 127
96  97  98  99  100 101 102 103 |   104 105 106 107 108 109 110 111
80  81  82  83  84  85  86  87  |   88  89  90  91  92  93  94  95
64  65  66  67  68  69  70  71  |   72  73  74  75  76  77  78  79
48  49  50  51  52  53  54  55  |   56  57  58  59  60  61  62  63
32  33  34  35  36  37  38  39  |   40  41  42  43  44  45  46  47
16  17  18  19  20  21  22  23  |   24  25  26  27  28  29  30  31
0   1   2   3   4   5   6   7   |   8   9   10  11  12  13  14  15
*/

#define VOID_PIECE 0

#define PAWN 1
#define ROOK 2
#define KNIGHT 3
#define BISHOP 4
#define QUEEN 5
#define KING 6

#define W_PAWN PAWN
#define W_ROOK ROOK
#define W_KNIGHT KNIGHT
#define W_BISHOP BISHOP
#define W_QUEEN QUEEN
#define W_KING KING

#define BLACK_PIECE_MASK 0x10
#define PIECE_MASK 0x7

#define B_PAWN PAWN | BLACK_PIECE_MASK
#define B_ROOK ROOK | BLACK_PIECE_MASK
#define B_KNIGHT KNIGHT | BLACK_PIECE_MASK
#define B_BISHOP BISHOP | BLACK_PIECE_MASK
#define B_QUEEN QUEEN | BLACK_PIECE_MASK
#define B_KING KING | BLACK_PIECE_MASK

struct Position {
  uint8_t mailbox[128]; // x88 mailbox flat array
  bool turn;            // true if white's turn
  bool white_kingside_castle;
  bool white_queenside_castle;
  bool black_kingside_castle;
  bool black_queenside_castle;
  int plies;
  int moves;
  uint8_t en_passant_square; // 0 if no en passant possible, is index of square
                             // otherwise
};

class PositionC {
  uint8_t mailbox[128]; // x88 mailbox flat array
  bool turn;            // true if white's turn
  bool white_kingside_castle;
  bool white_queenside_castle;
  bool black_kingside_castle;
  bool black_queenside_castle;
  int plies;
  int moves;
};

uint8_t an_square_to_index(std::string square);
std::string index_to_an_square(uint8_t index);
void print_position(std::shared_ptr<Position> position);
std::shared_ptr<Position> starting_position();

#endif