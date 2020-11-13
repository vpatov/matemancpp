#include "move_generation.hpp"
#include "position.hpp"
#include <assert.h>
#include <cstdint>
#include <iostream>
#include <vector>

/** Pseudolegal moves don't take check into account. */

#define ROOK_PROD(IS_YOUR_PIECE, IS_THEIR_PIECE, ADVANCE_DIRECTION)            \
  {                                                                            \
    inline void rook_prod(std::vector<uint8_t> moves, uint8_t square,          \
                          std::shared_ptr<Position> position) {                \
      uint8_t candidate = ADVANCE_DIRECTION(square);                           \
      do {                                                                     \
        uint8_t piece = position->mailbox[candidate];                          \
        if (IS_THEIR_PIECE(piece)) {                                           \
          moves.push_back(candidate);                                          \
          break;                                                               \
        }                                                                      \
        if (IS_YOUR_PIECE(piece)) {                                            \
          break;                                                               \
        }                                                                      \
        candidate = ADVANCE_DIRECTION(candidate);                              \
      } while (VALID_SQUARE(candidate));                                       \
    }                                                                          \
  }

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_pawn_moves(std::shared_ptr<Position> position,
                                uint8_t square) {

  assert(VALID_SQUARE(square));
  assert(position->mailbox[square] == PAWNC(C));

  uint8_t candidate;
  std::vector<uint8_t> moves;

  // check square in front
  candidate = FORWARD_RANK(C, square);
  if (VALID_SQUARE(candidate) && position->mailbox[candidate] == VOID_PIECE) {
    moves.push_back(candidate);

    // if square in front is empty, and we're on second rank, we can move two
    // squares
    candidate = FORWARD_RANK(C, candidate);
    if (IN_START_PAWN_RANK(C, square) && VALID_SQUARE(candidate) &&
        position->mailbox[candidate] == VOID_PIECE) {
      moves.push_back(candidate);
    }
  }

  // check diagonals for capture
  candidate = PREV_FILE(FORWARD_RANK(C, candidate));
  if (VALID_SQUARE(candidate) && IS_BLACK_PIECE(position->mailbox[candidate])) {
    moves.push_back(candidate);
  }

  candidate = NEXT_FILE(FORWARD_RANK(C, candidate));
  if (VALID_SQUARE(candidate) && IS_BLACK_PIECE(position->mailbox[candidate])) {
    moves.push_back(candidate);
  }

  return moves;
}

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_king_moves(std::shared_ptr<Position> position,
                                uint8_t square) {

  assert(VALID_SQUARE(square));
  assert(position->mailbox[square] == KINGC(C));

  uint8_t candidates[8] = {
      NEXT_RANK(PREV_FILE(square)),
      NEXT_RANK(square),
      NEXT_RANK(NEXT_FILE(square)),

      PREV_FILE(square),
      NEXT_FILE(square),

      PREV_RANK(PREV_FILE(square)),
      PREV_RANK(square),
      PREV_RANK(NEXT_FILE(square)),
  };
  std::vector<uint8_t> moves;

  for (int i = 0; i < 8; i++) {
    uint8_t candidate = candidates[i];
    uint8_t piece = position->mailbox[candidate];
    if (VALID_SQUARE(candidate) && (!IS_BLACK_PIECE(piece))) {
      moves.push_back(candidate);
    }
  }

  return moves;
}

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_knight_moves(std::shared_ptr<Position> position,
                                  uint8_t square) {
  assert(VALID_SQUARE(square));
  assert(position->mailbox[square] == KNIGHTC(C));

  uint8_t candidates[8] = {
      NEXT_RANK(PREV_FILE(PREV_FILE(square))),
      NEXT_RANK(NEXT_RANK(PREV_FILE(square))),

      PREV_RANK(PREV_FILE(PREV_FILE(square))),
      PREV_RANK(PREV_RANK(PREV_FILE(square))),

      NEXT_RANK(NEXT_FILE(NEXT_FILE(square))),
      NEXT_RANK(NEXT_RANK(NEXT_FILE(square))),

      PREV_RANK(NEXT_FILE(NEXT_FILE(square))),
      PREV_RANK(PREV_RANK(NEXT_FILE(square))),
  };
  std::vector<uint8_t> moves;

  for (int i = 0; i < 8; i++) {
    uint8_t candidate = candidates[i];
    uint8_t piece = position->mailbox[candidate];
    if (VALID_SQUARE(candidate) && IS_OPPONENT_PIECE(C, piece)) {
      moves.push_back(candidate);
    }
  }

  return moves;
}

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

template <Direction D, Color C>
inline void sliding_piece_walk(std::vector<uint8_t> *moves, uint8_t square,
                               std::shared_ptr<Position> position) {

  uint8_t candidate = STEP_DIRECTION(D, square);
  while (VALID_SQUARE(candidate)) {
    uint8_t piece = position->mailbox[candidate];
    if (IS_OPPONENT_PIECE(C, piece)) {
      moves->push_back(candidate);
      break;
    }
    if (IS_YOUR_PIECE(C, piece)) {
      break;
    }
    moves->push_back(candidate);
    candidate = STEP_DIRECTION(D, candidate);
  }
}

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_rook_moves(std::shared_ptr<Position> position,
                                uint8_t square) {

  assert(VALID_SQUARE(square));
  uint8_t candidate;
  std::vector<uint8_t> moves;

  sliding_piece_walk<Direction::UP, C>(&moves, square, position);
  sliding_piece_walk<Direction::DOWN, C>(&moves, square, position);
  sliding_piece_walk<Direction::RIGHT, C>(&moves, square, position);
  sliding_piece_walk<Direction::LEFT, C>(&moves, square, position);

  return moves;
}

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_bishop_moves(std::shared_ptr<Position> position,
                                  uint8_t square) {
  assert(VALID_SQUARE(square));
  uint8_t candidate;
  std::vector<uint8_t> moves;

  sliding_piece_walk<Direction::UPLEFT, C>(&moves, square, position);
  sliding_piece_walk<Direction::DOWNLEFT, C>(&moves, square, position);
  sliding_piece_walk<Direction::UPRIGHT, C>(&moves, square, position);
  sliding_piece_walk<Direction::DOWNRIGHT, C>(&moves, square, position);

  return moves;
}

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_queen_moves(std::shared_ptr<Position> position,
                                 uint8_t square) {
  assert(VALID_SQUARE(square));
  assert(position->mailbox[square] == QUEENC(C));
  std::vector<uint8_t> rook_moves;
  std::vector<uint8_t> bishop_moves;

  rook_moves = generate_pseudolegal_rook_moves<C>(position, square);
  bishop_moves = generate_pseudolegal_bishop_moves<C>(position, square);
  rook_moves.insert(rook_moves.end(), bishop_moves.begin(), bishop_moves.end());

  return rook_moves;
}

template std::vector<uint8_t> generate_pseudolegal_pawn_moves<Color::WHITE>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t> generate_pseudolegal_pawn_moves<Color::BLACK>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t> generate_pseudolegal_king_moves<Color::WHITE>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t> generate_pseudolegal_king_moves<Color::BLACK>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t> generate_pseudolegal_rook_moves<Color::WHITE>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t> generate_pseudolegal_rook_moves<Color::BLACK>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t> generate_pseudolegal_bishop_moves<Color::WHITE>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t> generate_pseudolegal_bishop_moves<Color::BLACK>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t> generate_pseudolegal_queen_moves<Color::WHITE>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t> generate_pseudolegal_queen_moves<Color::BLACK>(
    std::shared_ptr<Position> position, uint8_t square);