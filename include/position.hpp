#ifndef __POSITION_H__
#define __POSITION_H__

#include <cstdint>
#include <memory>
#include <string>

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

#define B_PAWN PAWN &BLACK_PIECE_MASK
#define B_ROOK ROOK &BLACK_PIECE_MASK
#define B_KNIGHT KNIGHT &BLACK_PIECE_MASK
#define B_BISHOP BISHOP &BLACK_PIECE_MASK
#define B_QUEEN QUEEN &BLACK_PIECE_MASK
#define B_KING KING &BLACK_PIECE_MASK

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

#endif