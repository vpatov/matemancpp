#include "move_generation.hpp"
#include <assert.h>
#include <cstdint>
#include <iostream>
#include <vector>

/** Pseudolegal moves don't take check into account. */

bool Position::is_move_legal(square_t src_square, square_t dst_square)
{
  // Assume the move
  // ----------------
  assert(is_valid_square(src_square));
  assert(is_valid_square(dst_square));

  Color color = m_whites_turn ? Color::WHITE : Color::BLACK;
  piece_t original_src_piece = m_mailbox[src_square];
  piece_t original_dst_piece = m_mailbox[dst_square];

  // m_mailbox[dst_square] =
  //     promotion_piece ? promotion_piece : m_mailbox[src_square];
  // we can ignore promotion because it doesnt affect the legality of the move
  // i.e. if it was legal to move the pawn up (pawn wasnt blocking check) then
  // it doesnt matter what piece it gets promoted to, it will be legal either
  // way.
  m_mailbox[dst_square] = m_mailbox[src_square];
  square_t square_of_pawn_being_captured = 0;

  if (m_en_passant_square &&
      m_en_passant_square == dst_square &&
      ((m_mailbox[src_square] == (m_whites_turn ? W_PAWN : B_PAWN))))
  {
    // Remove the pawn that is being captured
    square_of_pawn_being_captured = BACKWARD_RANK(color, dst_square);
    assert(m_mailbox[square_of_pawn_being_captured] == m_whites_turn
               ? B_PAWN
               : W_PAWN);
    m_mailbox[square_of_pawn_being_captured] = 0;
  }
  m_mailbox[src_square] = 0;

  bool legal = legal_position();

  // Undo the move
  // ----------------
  m_mailbox[src_square] = original_src_piece;
  m_mailbox[dst_square] = original_dst_piece;
  if (square_of_pawn_being_captured)
  {
    m_mailbox[square_of_pawn_being_captured] = m_whites_turn ? B_PAWN : W_PAWN;
  }
  return legal;
}

template <Color C>
std::vector<MoveKey>
generate_pseudolegal_pawn_moves(std::shared_ptr<Position> position,
                                square_t src_square)
{

  assert(is_valid_square(src_square));
  assert(position->m_mailbox[src_square] == PAWNC(C));

  square_t candidate_square;
  std::vector<MoveKey> moves;

  // check square in front
  candidate_square = FORWARD_RANK(C, src_square);
  if (is_valid_square(candidate_square) && is_empty(position->m_mailbox[candidate_square]))
  {

    // if this square is the last rank, then we must promote
    if (IN_LAST_PAWN_RANK_C(C, candidate_square))
    {
      moves.push_back(pack_move_key(src_square, candidate_square, QUEEN_C(C)));
      moves.push_back(pack_move_key(src_square, candidate_square, BISHOP_C(C)));
      moves.push_back(pack_move_key(src_square, candidate_square, KNIGHT_C(C)));
      moves.push_back(pack_move_key(src_square, candidate_square, ROOK_C(C)));
    }
    // otherwise just move to that rank
    else
    {
      moves.push_back(pack_move_key(src_square, candidate_square));
    }

    // if square in front is empty, and we're on second rank, we can move two
    // squares
    candidate_square = FORWARD_RANK(C, candidate_square);
    if (IN_START_PAWN_RANK(C, src_square) && is_valid_square(candidate_square) &&
        position->m_mailbox[candidate_square] == VOID_PIECE)
    {
      moves.push_back(pack_move_key(src_square, candidate_square));
    }
  }

  // check diagonals for capture
  candidate_square = PREV_FILE(FORWARD_RANK(C, src_square));
  if (is_valid_square(candidate_square) &&
      (IS_OPPONENT_PIECE(C, position->m_mailbox[candidate_square]) || position->m_en_passant_square == candidate_square))
  {
    moves.push_back(pack_move_key(src_square, candidate_square));
  }

  candidate_square = NEXT_FILE(FORWARD_RANK(C, src_square));
  if (is_valid_square(candidate_square) &&
      (IS_OPPONENT_PIECE(C, position->m_mailbox[candidate_square]) || position->m_en_passant_square == candidate_square))
  {
    moves.push_back(pack_move_key(src_square, candidate_square));
  }

  return moves;
}

template <Color C>
std::vector<MoveKey>
generate_pseudolegal_king_moves(std::shared_ptr<Position> position,
                                square_t src_square)
{

  assert(is_valid_square(src_square));
  assert(position->m_mailbox[src_square] == KINGC(C));

  square_t candidates[8] = {
      NEXT_RANK(PREV_FILE(src_square)),
      NEXT_RANK(src_square),
      NEXT_RANK(NEXT_FILE(src_square)),

      PREV_FILE(src_square),
      NEXT_FILE(src_square),

      PREV_RANK(PREV_FILE(src_square)),
      PREV_RANK(src_square),
      PREV_RANK(NEXT_FILE(src_square)),
  };
  std::vector<MoveKey> moves;

  for (int i = 0; i < 8; i++)
  {
    square_t candidate_square = candidates[i];
    piece_t piece = position->m_mailbox[candidate_square];
    if (is_valid_square(candidate_square) && (!IS_YOUR_PIECE(C, piece)))
    {
      moves.push_back(pack_move_key(src_square, candidate_square));
    }
  }
  auto castling_moves =
      generate_pseudolegal_castling_king_moves<C>(position, src_square);
  moves.insert(moves.end(), castling_moves.begin(), castling_moves.end());
  return moves;
}

#define KINGSIDE_CASTLE_C(C, position)             \
  (is_white(C) ? position->m_white_kingside_castle \
               : position->m_black_kingside_castle)
#define QUEENSIDE_CASTLE_C(C, position)             \
  (is_white(C) ? position->m_white_queenside_castle \
               : position->m_black_queenside_castle)

template <Color C>
std::vector<MoveKey>
generate_pseudolegal_castling_king_moves(std::shared_ptr<Position> position,
                                         square_t src_square)
{
  /** Assumes that position's castling booleans are correct. That is, king moves
   * and rook moves should immediately unset the respective castling boolean. */
  std::vector<MoveKey> moves;
  if (KINGSIDE_CASTLE_C(C, position) &&
      is_empty(position->m_mailbox[KING_KNIGHT_SQUARE_C(C)]) &&
      is_empty(position->m_mailbox[KING_BISHOP_SQUARE_C(C)]))
  {
    assert(position->m_mailbox[KING_SQUARE_C(C)] == KING_C(C));
    assert(position->m_mailbox[KING_ROOK_SQUARE_C(C)] == ROOK_C(C));
    moves.push_back(pack_move_key(src_square, KING_SHORT_CASTLE_SQUARE_C(C)));
  }
  if (QUEENSIDE_CASTLE_C(C, position) &&
      is_empty(position->m_mailbox[QUEEN_KNIGHT_SQUARE_C(C)]) &&
      is_empty(position->m_mailbox[QUEEN_SQUARE_C(C)]) &&
      is_empty(position->m_mailbox[QUEEN_BISHOP_SQUARE_C(C)]))
  {
    assert(position->m_mailbox[KING_SQUARE_C(C)] == KING_C(C));
    assert(position->m_mailbox[QUEEN_ROOK_SQUARE_C(C)] == ROOK_C(C));
    moves.push_back(pack_move_key(src_square, KING_LONG_CASTLE_SQUARE_C(C)));
  }
  return moves;
}

template <Color C>
std::vector<MoveKey>
generate_pseudolegal_knight_moves(std::shared_ptr<Position> position,
                                  square_t src_square)
{
  assert(is_valid_square(src_square));
  assert(position->m_mailbox[src_square] == KNIGHTC(C));

  square_t candidates[8] = {
      NEXT_RANK(PREV_FILE(PREV_FILE(src_square))),
      NEXT_RANK(NEXT_RANK(PREV_FILE(src_square))),

      PREV_RANK(PREV_FILE(PREV_FILE(src_square))),
      PREV_RANK(PREV_RANK(PREV_FILE(src_square))),

      NEXT_RANK(NEXT_FILE(NEXT_FILE(src_square))),
      NEXT_RANK(NEXT_RANK(NEXT_FILE(src_square))),

      PREV_RANK(NEXT_FILE(NEXT_FILE(src_square))),
      PREV_RANK(PREV_RANK(NEXT_FILE(src_square))),
  };
  std::vector<MoveKey> moves;

  for (int i = 0; i < 8; i++)
  {
    square_t candidate_square = candidates[i];
    piece_t piece = position->m_mailbox[candidate_square];
    if (is_valid_square(candidate_square) && !IS_YOUR_PIECE(C, piece))
    {
      moves.push_back(pack_move_key(src_square, candidate_square));
    }
  }
  return moves;
}

template <Direction D, Color C>
inline void sliding_piece_walk(std::vector<MoveKey> *moves, square_t src_square,
                               std::shared_ptr<Position> position)
{

  square_t candidate_square = STEP_DIRECTION(D, src_square);
  while (is_valid_square(candidate_square))
  {
    piece_t piece = position->m_mailbox[candidate_square];
    if (IS_OPPONENT_PIECE(C, piece))
    {
      moves->push_back(pack_move_key(src_square, candidate_square));
      break;
    }
    if (IS_YOUR_PIECE(C, piece))
    {
      break;
    }
    moves->push_back(pack_move_key(src_square, candidate_square));
    candidate_square = STEP_DIRECTION(D, candidate_square);
  }
}

template <Color C>
std::vector<MoveKey>
generate_pseudolegal_rook_moves(std::shared_ptr<Position> position,
                                square_t square)
{

  assert(is_valid_square(square));
  std::vector<MoveKey> moves;

  sliding_piece_walk<Direction::UP, C>(&moves, square, position);
  sliding_piece_walk<Direction::DOWN, C>(&moves, square, position);
  sliding_piece_walk<Direction::RIGHT, C>(&moves, square, position);
  sliding_piece_walk<Direction::LEFT, C>(&moves, square, position);

  return moves;
}

template <Color C>
std::vector<MoveKey>
generate_pseudolegal_bishop_moves(std::shared_ptr<Position> position,
                                  square_t src_square)
{
  assert(is_valid_square(src_square));
  std::vector<MoveKey> moves;

  sliding_piece_walk<Direction::UPLEFT, C>(&moves, src_square, position);
  sliding_piece_walk<Direction::DOWNLEFT, C>(&moves, src_square, position);
  sliding_piece_walk<Direction::UPRIGHT, C>(&moves, src_square, position);
  sliding_piece_walk<Direction::DOWNRIGHT, C>(&moves, src_square, position);

  return moves;
}

template <Color C>
std::vector<MoveKey>
generate_pseudolegal_queen_moves(std::shared_ptr<Position> position,
                                 square_t src_square)
{
  assert(is_valid_square(src_square));
  assert(position->m_mailbox[src_square] == QUEENC(C));
  std::vector<MoveKey> rook_moves;
  std::vector<MoveKey> bishop_moves;

  rook_moves = generate_pseudolegal_rook_moves<C>(position, src_square);
  bishop_moves = generate_pseudolegal_bishop_moves<C>(position, src_square);
  rook_moves.insert(rook_moves.end(), bishop_moves.begin(), bishop_moves.end());

  return rook_moves;
}

std::vector<MoveKey>
generate_legal_moves(std::shared_ptr<Position> position,
                     square_t src_square)
{

  // all possible moves (not taking discovered check into account)
  auto moves = generate_pseudolegal_piece_moves(position, src_square);
  std::vector<MoveKey> legal_moves;

  // need to filter moves by legality
  // naive way: for each move,  assume move, and check underlying position for legality

  // maybe en passant and promotion can be represented in the moves returned by pseudolegal move generation
  // just returning squares makes it tricky to think about those two.

  for (auto it = moves.begin(); it != moves.end(); it++)
  {
    auto move = unpack_move_key(*it);
    auto dst_square = move.m_dst_square;

    // easier to assume move and check for legality
    if (position->is_move_legal(src_square, dst_square))
    {
      legal_moves.push_back(*it);
    }
    // assume_position()
  }

  return legal_moves;
}

template <Color C>
std::vector<MoveKey>
generate_pseudolegal_piece_moves(std::shared_ptr<Position> position,
                                 square_t square)
{
  uint8_t piece = position->m_mailbox[square];
  switch (piece & PIECE_MASK)
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

std::vector<MoveKey>
generate_pseudolegal_piece_moves(std::shared_ptr<Position> position,
                                 square_t src_square)
{
  piece_t piece = position->m_mailbox[src_square];
  return is_white_piece(piece)
             ? generate_pseudolegal_piece_moves<Color::WHITE>(position, src_square)
             : generate_pseudolegal_piece_moves<Color::BLACK>(position, src_square);
}

template std::vector<MoveKey> generate_pseudolegal_pawn_moves<Color::WHITE>(
    std::shared_ptr<Position> position, square_t src_square);

template std::vector<MoveKey> generate_pseudolegal_pawn_moves<Color::BLACK>(
    std::shared_ptr<Position> position, square_t src_square);

template std::vector<MoveKey> generate_pseudolegal_king_moves<Color::WHITE>(
    std::shared_ptr<Position> position, square_t src_square);

template std::vector<MoveKey> generate_pseudolegal_king_moves<Color::BLACK>(
    std::shared_ptr<Position> position, square_t src_square);

template std::vector<MoveKey>
generate_pseudolegal_castling_king_moves<Color::WHITE>(
    std::shared_ptr<Position> position, square_t src_square);

template std::vector<MoveKey>
generate_pseudolegal_castling_king_moves<Color::BLACK>(
    std::shared_ptr<Position> position, square_t src_square);

template std::vector<MoveKey> generate_pseudolegal_rook_moves<Color::WHITE>(
    std::shared_ptr<Position> position, square_t src_square);

template std::vector<MoveKey> generate_pseudolegal_rook_moves<Color::BLACK>(
    std::shared_ptr<Position> position, square_t src_square);

template std::vector<MoveKey> generate_pseudolegal_bishop_moves<Color::WHITE>(
    std::shared_ptr<Position> position, square_t src_square);

template std::vector<MoveKey> generate_pseudolegal_bishop_moves<Color::BLACK>(
    std::shared_ptr<Position> position, square_t src_square);

template std::vector<MoveKey> generate_pseudolegal_queen_moves<Color::WHITE>(
    std::shared_ptr<Position> position, square_t src_square);

template std::vector<MoveKey> generate_pseudolegal_queen_moves<Color::BLACK>(
    std::shared_ptr<Position> position, square_t src_square);

template std::vector<MoveKey> generate_pseudolegal_piece_moves<Color::WHITE>(
    std::shared_ptr<Position> position, square_t src_square);

template std::vector<MoveKey> generate_pseudolegal_piece_moves<Color::BLACK>(
    std::shared_ptr<Position> position, square_t src_square);

bool white_attacks_diagonally(piece_t piece)
{
  return is_w_bishop(piece) || is_w_queen(piece);
}

bool black_attacks_diagonally(piece_t piece)
{
  return is_b_bishop(piece) || is_b_queen(piece);
}

bool white_attacks_files_ranks(piece_t piece)
{
  return is_w_rook(piece) || is_w_queen(piece);
}

bool black_attacks_files_ranks(piece_t piece)
{
  return is_b_rook(piece) || is_b_queen(piece);
}

bool is_w_pawn(piece_t piece)
{
  return piece == W_PAWN;
}
bool is_b_pawn(piece_t piece)
{
  return piece == B_PAWN;
}

bool is_w_bishop(piece_t piece)
{
  return piece == W_BISHOP;
}
bool is_w_rook(piece_t piece)
{
  return piece == W_ROOK;
}
bool is_w_queen(piece_t piece)
{
  return piece == W_QUEEN;
}
bool is_b_bishop(piece_t piece)
{
  return piece == B_BISHOP;
}
bool is_b_rook(piece_t piece)
{
  return piece == B_ROOK;
}
bool is_b_queen(piece_t piece)
{
  return piece == B_QUEEN;
}