#ifndef __MOVE_GENERATION_H__
#define __MOVE_GENERATION_H__
#include "position.hpp"
#include <cstdint>
#include <vector>

enum class Color { WHITE, BLACK };
enum Direction { UP, DOWN, LEFT, RIGHT, UPLEFT, UPRIGHT, DOWNLEFT, DOWNRIGHT };

constexpr bool white_mgen(Color C) { return C == Color::WHITE; }

#define PAWNC(C) (white_mgen(C) ? W_PAWN : B_PAWN)
#define ROOKC(C) (white_mgen(C) ? W_ROOK : B_ROOK)
#define KNIGHTC(C) (white_mgen(C) ? W_KNIGHT : B_KNIGHT)
#define BISHOPC(C) (white_mgen(C) ? W_BISHOP : B_BISHOP)
#define QUEENC(C) (white_mgen(C) ? W_QUEEN : B_QUEEN)
#define KINGC(C) (white_mgen(C) ? W_KING : B_KING)

#define KING_ROOK_SQUARE_C(C)                                                  \
  (white_mgen(C) ? W_KING_ROOK_SQUARE : B_KING_ROOK_SQUARE)
#define QUEEN_ROOK_SQUARE_C(C)                                                 \
  (white_mgen(C) ? W_QUEEN_ROOK_SQUARE : B_QUEEN_ROOK_SQUARE)

#define RANK_OFFSET 16
#define FILE_OFFSET 1

#define NEXT_RANK(sq) (static_cast<uint8_t>(sq + RANK_OFFSET))
#define PREV_RANK(sq) (static_cast<uint8_t>(sq - RANK_OFFSET))

#define PREV_FILE(sq) (static_cast<uint8_t>(sq - FILE_OFFSET))
#define NEXT_FILE(sq) (static_cast<uint8_t>(sq + FILE_OFFSET))

// Returns the rank that is forward relative to the player
#define FORWARD_RANK(C, square)                                                \
  (white_mgen(C) ? NEXT_RANK(square) : PREV_RANK(square))

#define IN_SECOND_RANK(sq) (sq >= 16 && sq <= 23)
#define IN_SEVENTH_RANK(sq) (sq >= 96 && sq <= 103)
#define IN_START_PAWN_RANK(C, square)                                          \
  (white_mgen(C) ? IN_SECOND_RANK(square) : IN_SEVENTH_RANK(square))

#define IS_YOUR_PIECE(C, piece)                                                \
  (white_mgen(C) ? IS_WHITE_PIECE(piece) : IS_BLACK_PIECE(piece))
#define IS_OPPONENT_PIECE(C, piece)                                            \
  (white_mgen(C) ? IS_BLACK_PIECE(piece) : IS_WHITE_PIECE(piece))

constexpr uint8_t direction_offset(Direction D) {
  switch (D) {
  case Direction::UP:
    return NEXT_RANK(0);
  case Direction::DOWN:
    return PREV_RANK(0);
  case Direction::RIGHT:
    return NEXT_FILE(0);
  case Direction::LEFT:
    return PREV_FILE(0);
  case Direction::UPLEFT:
    return PREV_FILE(NEXT_RANK(0));
  case Direction::UPRIGHT:
    return NEXT_FILE(NEXT_RANK(0));
  case Direction::DOWNLEFT:
    return PREV_FILE(PREV_RANK(0));
  case Direction::DOWNRIGHT:
    return NEXT_FILE(PREV_RANK(0));
  default:
    __builtin_unreachable();
  }
}

#define STEP_DIRECTION(D, square) (direction_offset(D) + square)

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_pawn_moves(std::shared_ptr<Position> position,
                                uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_king_moves(std::shared_ptr<Position> position,
                                uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_castling_king_moves(std::shared_ptr<Position> position,
                                         uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_knight_moves(std::shared_ptr<Position> position,
                                  uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_rook_moves(std::shared_ptr<Position> position,
                                uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_bishop_moves(std::shared_ptr<Position> position,
                                  uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_queen_moves(std::shared_ptr<Position> position,
                                 uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_piece_moves(std::shared_ptr<Position> position,
                                 uint8_t square);

std::vector<uint8_t>
generate_pseudolegal_piece_moves(std::shared_ptr<Position> position,
                                 uint8_t square);

#endif