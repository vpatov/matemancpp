#include "representation/position.hpp"
#include "representation/notation.hpp"
#include "move_generation.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <memory>

std::unique_ptr<Position> generate_starting_position()
{
  std::unique_ptr<Position> position = std::make_unique<Position>();
  populate_starting_position(position.get());
  return std::move(position);
}

std::shared_ptr<Position> starting_position()
{
  auto position = std::make_shared<Position>();
  populate_starting_position(position.get());

  return position;
}

void populate_starting_position(Position *position)
{
  std::fill(position->m_mailbox, (position->m_mailbox) + 128, 0);

  /** White pieces*/
  position->m_mailbox[0x0] = W_ROOK;
  position->m_mailbox[0x1] = W_KNIGHT;
  position->m_mailbox[0x2] = W_BISHOP;
  position->m_mailbox[0x3] = W_QUEEN;
  position->m_mailbox[0x4] = W_KING;
  position->m_mailbox[0x5] = W_BISHOP;
  position->m_mailbox[0x6] = W_KNIGHT;
  position->m_mailbox[0x7] = W_ROOK;
  for (int i = 0x10; i < 0x18; i++)
  {
    position->m_mailbox[i] = W_PAWN;
  }

  /** Black pieces*/
  position->m_mailbox[0x70] = B_ROOK;
  position->m_mailbox[0x71] = B_KNIGHT;
  position->m_mailbox[0x72] = B_BISHOP;
  position->m_mailbox[0x73] = B_QUEEN;
  position->m_mailbox[0x74] = B_KING;
  position->m_mailbox[0x75] = B_BISHOP;
  position->m_mailbox[0x76] = B_KNIGHT;
  position->m_mailbox[0x77] = B_ROOK;
  for (int i = 0x60; i < 0x68; i++)
  {
    position->m_mailbox[i] = B_PAWN;
  }

  position->m_white_kingside_castle = true;
  position->m_white_queenside_castle = true;
  position->m_black_kingside_castle = true;
  position->m_black_queenside_castle = true;

  position->m_plies = 0;
  position->m_moves = 1;
  position->m_en_passant_square = INVALID_SQUARE;
  position->m_whites_turn = true;
}

void print_position(Position *position)
{
  int i = 0x70; //0x70 is the top-left corner of the board
  while (1)
  {
    std::cout << piece_to_char(position->m_mailbox[i]) << " ";
    i++;
    if (i == 8)
    {
      std::cout << std::endl
                << std::endl;
      return;
    }
    if (i & 0x88)
    {
      i -= 0x18;
      std::cout << std::endl;
    }
  }
}

void print_position_with_borders(Position *position)
{
  int i = 0x70;
  char rank = '8';

  std::cout << "   ";
  for (char file = 'a'; file <= 'h'; file++)
  {
    std::cout << file << " ";
  }
  std::cout << std::endl
            << std::endl;

  while (1)
  {
    if (i % 16 == 0)
    {
      std::cout << rank << "  ";
    }
    std::cout << piece_to_color_coded_char(position->m_mailbox[i], false) << " ";
    i++;

    if (i & 0x88)
    {
      i -= 0x18;
      std::cout << "  " << rank-- << std::endl;
    }
    if (i == (8 - 0x18))
    {
      break;
    }
  }
  std::cout << std::endl;
  std::cout << "   ";
  for (char file = 'a'; file <= 'h'; file++)
  {
    std::cout << file << " ";
  }
  std::cout << std::endl
            << std::endl;
}

std::string Position::pretty_string()
{
  std::stringstream ss;
  int i = 0x70;
  char rank = '8';

  ss << '\n';
  ss << "   ";
  for (char file = 'a'; file <= 'h'; file++)
  {
    ss << file << " ";
  }
  ss << "\n\n";

  while (1)
  {
    if (i % 16 == 0)
    {
      ss << rank << "  ";
    }
    ss << piece_to_char(m_mailbox[i]) << " ";
    i++;

    if (i & 0x88)
    {
      i -= 0x18;
      ss << "  " << rank-- << '\n';
    }
    if (i == (8 - 0x18))
    {
      break;
    }
  }
  ss << '\n';
  ss << "   ";
  for (char file = 'a'; file <= 'h'; file++)
  {
    ss << file << " ";
  }
  ss << '\n'
     << std::endl;
  return ss.str();
}

void Position::print_with_borders_highlight_squares(square_t src_square, square_t dest_square)
{

  int i = 0x70;
  char rank = '8';

  std::cout << "   ";
  for (char file = 'a'; file <= 'h'; file++)
  {
    std::cout << file << " ";
  }
  std::cout << std::endl
            << std::endl;

  while (1)
  {
    if (i % 16 == 0)
    {
      std::cout << rank << "  ";
    }
    if (i == src_square)
    {
      std::cout << piece_to_color_coded_char(m_mailbox[i], true) << " ";
    }
    else if (i == dest_square)
    {
      std::cout << piece_to_color_coded_char(m_mailbox[i], true) << " ";
    }
    else
    {
      std::cout << piece_to_color_coded_char(m_mailbox[i], false) << " ";
    }
    i++;

    if (i & 0x88)
    {
      i -= 0x18;
      std::cout << "  " << rank-- << std::endl;
    }
    if (i == (8 - 0x18))
    {
      break;
    }
  }
  std::cout << std::endl;
  std::cout << "   ";
  for (char file = 'a'; file <= 'h'; file++)
  {
    std::cout << file << " ";
  }
  std::cout << std::endl
            << std::endl;
}

// returns a vector containing all the squares of enemy pieces that diagonally attack the target square.
// TODO define separate types for squares and pieces, because uint8_t everywhere is confusing.
std::vector<uint8_t> check_diagonals(Position *position, square_t target_square, bool color_of_attackers)
{
  std::vector<square_t> squares;
  //look for bishops/queens attacking square on diagonals
  for (int i = 0; i < 4; i++)
  {
    square_t square = check_line(
        position,
        target_square,
        bishop_offsets[i],
        color_of_attackers ? &white_attacks_diagonally : &black_attacks_diagonally);
    if (is_valid_square(square))
    {
      squares.push_back(square);
    }
  }

  return squares;
}

std::vector<square_t> find_attacking_knights(Position *position, square_t target_square, bool color_of_attackers)
{
  std::vector<square_t> squares;
  for (auto it = knight_move_offsets.begin(); it != knight_move_offsets.end(); it++)
  {
    square_t square = *it + target_square;
    if (is_invalid_square(square))
    {
      continue;
    }
    if (position->m_mailbox[square] == (color_of_attackers ? W_KNIGHT : B_KNIGHT))
    {
      squares.push_back(square);
    }
  }
  return squares;
}

std::vector<square_t> find_attacking_bishops(Position *position, square_t target_square, bool color_of_attackers)
{
  std::vector<square_t> squares;
  //look for bishops attacking square on diagonals
  for (int i = 0; i < 4; i++)
  {
    square_t square = check_line(
        position,
        target_square,
        bishop_offsets[i],
        color_of_attackers ? &is_w_bishop : &is_b_bishop);
    if (is_valid_square(square))
    {
      squares.push_back(square);
    }
  }

  return squares;
}

std::vector<square_t> find_attacking_rooks(Position *position, square_t target_square, bool color_of_attackers)
{
  std::vector<square_t> squares;
  //look for rooks attacking square on diagonals
  for (int i = 0; i < 4; i++)
  {
    square_t square = check_line(position, target_square, rook_offsets[i], color_of_attackers ? &is_w_rook : &is_b_rook);
    if (is_valid_square(square))
    {
      squares.push_back(square);
    }
  }

  return squares;
}

std::vector<square_t> find_attacking_queens(Position *position, square_t target_square, bool color_of_attackers)
{
  std::vector<square_t> squares;
  //look for queens attacking square on diagonals
  for (int i = 0; i < 4; i++)
  {
    square_t square = check_line(
        position,
        target_square,
        bishop_offsets[i],
        color_of_attackers ? &is_w_queen : &is_b_queen);
    if (is_valid_square(square))
    {
      squares.push_back(square);
    }
    square = check_line(
        position,
        target_square,
        rook_offsets[i],
        color_of_attackers ? &is_w_queen : &is_b_queen);
    if (is_valid_square(square))
    {
      squares.push_back(square);
    }
  }

  return squares;
}

// returns a vector containing all the squares of enemy pieces that diagonally attack the target square.
// TODO define separate types for squares and pieces, because uint8_t everywhere is confusing.
std::vector<square_t> check_files_ranks(Position *position, square_t target_square, bool color_of_attackers)
{
  std::vector<square_t> squares;
  //look for rooks/queens attacking square on files and ranks
  for (int i = 0; i < 4; i++)
  {
    square_t square = check_line(
        position,
        target_square,
        rook_offsets[i],
        color_of_attackers ? &white_attacks_files_ranks : &black_attacks_files_ranks);
    if (is_valid_square(square))
    {
      squares.push_back(square);
    }
  }

  return squares;
}

// returns the square of the first piece that attacks the target along the line.
square_t check_line(Position *position, square_t target, int offset, bool (*piece_type_function)(piece_t))
{
  for (square_t candidate = target + offset; is_valid_square(candidate); candidate += offset)
  {
    piece_t piece = position->m_mailbox[candidate];
    // square is empty so we need to keep looking.
    if (piece == 0)
    {
      continue;
    }

    // square contains an enemy piece that can attack along this line
    if (piece_type_function(position->m_mailbox[candidate]))
    {
      return candidate;
    }
    // square is not empty and contains something that doesn't attack the target, and thus blocks the line.
    else
    {
      return INVALID_SQUARE;
    }
  }
  // This is reached if all squares in the line were empty.
  return INVALID_SQUARE;
}

bool check_diagonal_or_file_or_rank(Position *position, square_t king_square, int offset, piece_t target1, piece_t target2)
{
  for (square_t candidate = king_square + offset; is_valid_square(candidate); candidate += offset)
  {
    piece_t piece = position->m_mailbox[candidate];
    // square is empty so we need to keep looking.
    if (piece == 0)
    {
      continue;
    }

    // square contains either the enemy bishop or queen.
    if (position->m_mailbox[candidate] == target1 || position->m_mailbox[candidate] == target2)
    {
      return false;
    }

    // square is not empty, and contains something other than an enemy bishop or queen, therefore it will
    // block attacks along this diagonal, we can break.
    else
    {
      break;
    }
  }
  return true;
}

void Position::advance_position(Move move)
{
  return advance_position(move.m_src_square, move.m_dst_square, move.m_promotion_piece);
}

// given a long algebraic notation move, make the move.
// position, plies, en passant, whose turn
// should not contain an assertion for legality. this will be handled later down the line
// by the move-generation functions, and it can be checked that the output of this is within
// the list of legal moves.
void Position::advance_position(square_t src_square, square_t dst_square, piece_t promotion_piece)
{
  piece_t moving_piece = m_mailbox[src_square];
  piece_t captured_piece = m_mailbox[dst_square];
  square_t new_en_passant_square = INVALID_SQUARE;

  Color C = m_whites_turn ? Color::WHITE : Color::BLACK;
  assert(moving_piece != VOID_PIECE);
  assert(IS_YOUR_PIECE(C, moving_piece));

  // remove castling rights if the rook moves or gets captured
  if (src_square == W_KING_ROOK_SQUARE || dst_square == W_KING_ROOK_SQUARE)
  {
    m_white_kingside_castle = false;
  }
  else if (src_square == W_QUEEN_ROOK_SQUARE || dst_square == W_QUEEN_ROOK_SQUARE)
  {
    m_white_queenside_castle = false;
  }
  else if (src_square == B_KING_ROOK_SQUARE || dst_square == B_KING_ROOK_SQUARE)
  {
    m_black_kingside_castle = false;
  }
  else if (src_square == B_QUEEN_ROOK_SQUARE || dst_square == B_QUEEN_ROOK_SQUARE)
  {
    m_black_queenside_castle = false;
  }

  else if (moving_piece == KING_C(C) && src_square == KING_SQUARE_C(C))
  {
    if (m_whites_turn)
    {
      m_white_kingside_castle = false;
      m_white_queenside_castle = false;
    }
    else
    {
      m_black_kingside_castle = false;
      m_black_queenside_castle = false;
    }
    if (dst_square == KING_SHORT_CASTLE_SQUARE_C(C))
    {
      m_mailbox[KING_ROOK_SQUARE_C(C)] = 0;
      m_mailbox[ROOK_SHORT_CASTLE_SQUARE_C(C)] = ROOK_C(C);
    }
    else if (dst_square == KING_LONG_CASTLE_SQUARE_C(C))
    {
      m_mailbox[QUEEN_ROOK_SQUARE_C(C)] = 0;
      m_mailbox[ROOK_LONG_CASTLE_SQUARE_C(C)] = ROOK_C(C);
    }
  }
  // -----------

  // if the promotion piece is set, and its not whites turn,
  // we have to turn the promotion piece into a black piece.
  if (promotion_piece && !m_whites_turn)
  {
    promotion_piece |= BLACK_PIECE_MASK;
  }

  m_mailbox[dst_square] =
      promotion_piece ? promotion_piece : m_mailbox[src_square];

  // if we are capturing en passant
  if (is_valid_square(m_en_passant_square) &&
      m_en_passant_square == dst_square &&
      ((m_mailbox[src_square] == (PAWN_C(C)))))
  {

    // If we are capturing en-passant there should never be a promotion piece
    assert(!promotion_piece);

    // Remove the pawn that is being captured
    uint8_t square_of_pawn_being_captured = BACKWARD_RANK(C, dst_square);
    assert(m_mailbox[square_of_pawn_being_captured] == m_whites_turn
               ? B_PAWN
               : W_PAWN);
    m_mailbox[square_of_pawn_being_captured] = 0;
  }

  // if pawn is advancing two squares, set the en passant square
  if (m_mailbox[src_square] == (m_whites_turn ? W_PAWN : B_PAWN) &&
      dst_square == FORWARD_RANK(C, FORWARD_RANK(C, src_square)))
  {
    new_en_passant_square = FORWARD_RANK(C, src_square);
  }

  m_en_passant_square = new_en_passant_square;

  m_whites_turn = !m_whites_turn;
  m_plies++;
  m_mailbox[src_square] = 0;
}