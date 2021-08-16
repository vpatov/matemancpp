#include "move_generation.hpp"
#include "position.hpp"
#include <assert.h>
#include <cstdint>
#include <iostream>
#include <vector>

/** Pseudolegal moves don't take check into account. */

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_pawn_moves(std::shared_ptr<Position> position,
                                uint8_t square)
{

  assert(IS_VALID_SQUARE(square));
  assert(position->mailbox[square] == PAWNC(C));

  uint8_t candidate;
  std::vector<uint8_t> moves;

  // check square in front
  candidate = FORWARD_RANK(C, square);
  if (IS_VALID_SQUARE(candidate) && position->mailbox[candidate] == VOID_PIECE)
  {
    moves.push_back(candidate);

    // if square in front is empty, and we're on second rank, we can move two
    // squares
    candidate = FORWARD_RANK(C, candidate);
    if (IN_START_PAWN_RANK(C, square) && IS_VALID_SQUARE(candidate) &&
        position->mailbox[candidate] == VOID_PIECE)
    {
      moves.push_back(candidate);
    }
  }

  // check diagonals for capture
  candidate = PREV_FILE(FORWARD_RANK(C, candidate));
  if (IS_VALID_SQUARE(candidate) &&
      IS_OPPONENT_PIECE(C, position->mailbox[candidate]))
  {
    moves.push_back(candidate);
  }

  candidate = NEXT_FILE(FORWARD_RANK(C, candidate));
  if (IS_VALID_SQUARE(candidate) &&
      IS_OPPONENT_PIECE(C, position->mailbox[candidate]))
  {
    moves.push_back(candidate);
  }

  return moves;
}

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_king_moves(std::shared_ptr<Position> position,
                                uint8_t square)
{

  assert(IS_VALID_SQUARE(square));
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

  for (int i = 0; i < 8; i++)
  {
    uint8_t candidate = candidates[i];
    uint8_t piece = position->mailbox[candidate];
    if (IS_VALID_SQUARE(candidate) && (!IS_BLACK_PIECE(piece)))
    {
      moves.push_back(candidate);
    }
  }
  auto castling_moves =
      generate_pseudolegal_castling_king_moves<C>(position, square);
  moves.insert(moves.end(), castling_moves.begin(), castling_moves.end());
  return moves;
}

#define KINGSIDE_CASTLE_C(C, position)             \
  (white_mgen(C) ? position->white_kingside_castle \
                 : position->black_kingside_castle)
#define QUEENSIDE_CASTLE_C(C, position)             \
  (white_mgen(C) ? position->white_queenside_castle \
                 : position->black_queenside_castle)

// TODO
template <Color C>
std::vector<uint8_t>
generate_pseudolegal_castling_king_moves(std::shared_ptr<Position> position,
                                         uint8_t square)
{
  /** Assumes that position's castling booleans are correct. That is, king moves
   * and rook moves should immediately unset the respective castling boolean. */
  std::vector<uint8_t> moves;
  if (KINGSIDE_CASTLE_C(C, position))
  {
    moves.push_back(KING_ROOK_SQUARE_C(C));
  }
  if (QUEENSIDE_CASTLE_C(C, position))
  {
    moves.push_back(QUEEN_ROOK_SQUARE_C(C));
  }
  return moves;
}

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_knight_moves(std::shared_ptr<Position> position,
                                  uint8_t square)
{
  assert(IS_VALID_SQUARE(square));
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

  for (int i = 0; i < 8; i++)
  {
    uint8_t candidate = candidates[i];
    uint8_t piece = position->mailbox[candidate];
    if (IS_VALID_SQUARE(candidate) && IS_OPPONENT_PIECE(C, piece))
    {
      moves.push_back(candidate);
    }
  }

  return moves;
}

template <Direction D, Color C>
inline void sliding_piece_walk(std::vector<uint8_t> *moves, uint8_t square,
                               std::shared_ptr<Position> position)
{

  uint8_t candidate = STEP_DIRECTION(D, square);
  while (IS_VALID_SQUARE(candidate))
  {
    uint8_t piece = position->mailbox[candidate];
    if (IS_OPPONENT_PIECE(C, piece))
    {
      moves->push_back(candidate);
      break;
    }
    if (IS_YOUR_PIECE(C, piece))
    {
      break;
    }
    moves->push_back(candidate);
    candidate = STEP_DIRECTION(D, candidate);
  }
}

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_rook_moves(std::shared_ptr<Position> position,
                                uint8_t square)
{

  assert(IS_VALID_SQUARE(square));
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
                                  uint8_t square)
{
  assert(IS_VALID_SQUARE(square));
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
                                 uint8_t square)
{
  assert(IS_VALID_SQUARE(square));
  assert(position->mailbox[square] == QUEENC(C));
  std::vector<uint8_t> rook_moves;
  std::vector<uint8_t> bishop_moves;

  rook_moves = generate_pseudolegal_rook_moves<C>(position, square);
  bishop_moves = generate_pseudolegal_bishop_moves<C>(position, square);
  rook_moves.insert(rook_moves.end(), bishop_moves.begin(), bishop_moves.end());

  return rook_moves;
}

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_piece_moves(std::shared_ptr<Position> position,
                                 uint8_t square)
{
  uint8_t piece = position->mailbox[square];
  switch (piece)
  {
  case PAWN:
    return generate_pseudolegal_pawn_moves<C>(position, square);
  case ROOK:
    return generate_pseudolegal_rook_moves<C>(position, square);
  case KNIGHT:
    return generate_pseudolegal_knight_moves<C>(position, square);
  case BISHOP:
    return generate_pseudolegal_bishop_moves<C>(position, square);
  case QUEEN:
    return generate_pseudolegal_queen_moves<C>(position, square);
  case KING:
    return generate_pseudolegal_king_moves<C>(position, square);
  default:
    __builtin_unreachable();
  }
}

std::vector<uint8_t>
generate_pseudolegal_piece_moves(std::shared_ptr<Position> position,
                                 uint8_t square)
{
  uint8_t piece = position->mailbox[square];
  return IS_WHITE_PIECE(piece)
             ? generate_pseudolegal_piece_moves<Color::WHITE>(position, square)
             : generate_pseudolegal_piece_moves<Color::BLACK>(position, square);
}

template std::vector<uint8_t> generate_pseudolegal_pawn_moves<Color::WHITE>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t> generate_pseudolegal_pawn_moves<Color::BLACK>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t> generate_pseudolegal_king_moves<Color::WHITE>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t> generate_pseudolegal_king_moves<Color::BLACK>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t>
generate_pseudolegal_castling_king_moves<Color::WHITE>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t>
generate_pseudolegal_castling_king_moves<Color::BLACK>(
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

template std::vector<uint8_t> generate_pseudolegal_piece_moves<Color::WHITE>(
    std::shared_ptr<Position> position, uint8_t square);

template std::vector<uint8_t> generate_pseudolegal_piece_moves<Color::BLACK>(
    std::shared_ptr<Position> position, uint8_t square);

bool white_attacks_diagonally(uint8_t piece)
{
  return is_w_bishop(piece) || is_w_queen(piece);
}

bool black_attacks_diagonally(uint8_t piece)
{
  return is_b_bishop(piece) || is_b_queen(piece);
}

bool white_attacks_files_ranks(uint8_t piece)
{
  return is_w_rook(piece) || is_w_queen(piece);
}

bool black_attacks_files_ranks(uint8_t piece)
{
  return is_b_rook(piece) || is_b_queen(piece);
}

bool is_w_bishop(uint8_t piece)
{
  return piece == W_BISHOP;
}
bool is_w_rook(uint8_t piece)
{
  return piece == W_ROOK;
}
bool is_w_queen(uint8_t piece)
{
  return piece == W_QUEEN;
}
bool is_b_bishop(uint8_t piece)
{
  return piece == B_BISHOP;
}
bool is_b_rook(uint8_t piece)
{
  return piece == B_ROOK;
}
bool is_b_queen(uint8_t piece)
{
  return piece == B_QUEEN;
}