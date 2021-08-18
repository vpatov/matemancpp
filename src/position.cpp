#include "position.hpp"
#include "move_generation.hpp"
#include <iostream>
#include <algorithm>
#include <memory>

std::shared_ptr<Position> starting_position()
{
  auto position = std::make_shared<Position>();

  /** White pieces*/
  position->mailbox[0x0] = W_ROOK;
  position->mailbox[0x1] = W_KNIGHT;
  position->mailbox[0x2] = W_BISHOP;
  position->mailbox[0x3] = W_QUEEN;
  position->mailbox[0x4] = W_KING;
  position->mailbox[0x5] = W_BISHOP;
  position->mailbox[0x6] = W_KNIGHT;
  position->mailbox[0x7] = W_ROOK;
  for (int i = 0x10; i < 0x18; i++)
  {
    position->mailbox[i] = W_PAWN;
  }

  /** Black pieces*/
  position->mailbox[0x70] = B_ROOK;
  position->mailbox[0x71] = B_KNIGHT;
  position->mailbox[0x72] = B_BISHOP;
  position->mailbox[0x73] = B_QUEEN;
  position->mailbox[0x74] = B_KING;
  position->mailbox[0x75] = B_BISHOP;
  position->mailbox[0x76] = B_KNIGHT;
  position->mailbox[0x77] = B_ROOK;
  for (int i = 0x60; i < 0x68; i++)
  {
    position->mailbox[i] = B_PAWN;
  }

  position->white_kingside_castle = true;
  position->white_queenside_castle = true;
  position->black_kingside_castle = true;
  position->black_queenside_castle = true;

  position->plies = 0;
  position->moves = 1;
  position->en_passant_square = 0;
  position->turn = true;

  return position;
}

void populate_starting_position(Position *position)
{
  std::fill(position->mailbox, (position->mailbox) + 128, 0);

  /** White pieces*/
  position->mailbox[0x0] = W_ROOK;
  position->mailbox[0x1] = W_KNIGHT;
  position->mailbox[0x2] = W_BISHOP;
  position->mailbox[0x3] = W_QUEEN;
  position->mailbox[0x4] = W_KING;
  position->mailbox[0x5] = W_BISHOP;
  position->mailbox[0x6] = W_KNIGHT;
  position->mailbox[0x7] = W_ROOK;
  for (int i = 0x10; i < 0x18; i++)
  {
    position->mailbox[i] = W_PAWN;
  }

  /** Black pieces*/
  position->mailbox[0x70] = B_ROOK;
  position->mailbox[0x71] = B_KNIGHT;
  position->mailbox[0x72] = B_BISHOP;
  position->mailbox[0x73] = B_QUEEN;
  position->mailbox[0x74] = B_KING;
  position->mailbox[0x75] = B_BISHOP;
  position->mailbox[0x76] = B_KNIGHT;
  position->mailbox[0x77] = B_ROOK;
  for (int i = 0x60; i < 0x68; i++)
  {
    position->mailbox[i] = B_PAWN;
  }

  position->white_kingside_castle = true;
  position->white_queenside_castle = true;
  position->black_kingside_castle = true;
  position->black_queenside_castle = true;

  position->plies = 0;
  position->moves = 1;
  position->en_passant_square = 0;
  position->turn = true;
}

uint8_t an_square_to_index(std::string square)
{
  return (square.at(0) - 'a') + ((square.at(1) - '1') * 0x10);
}

uint8_t an_square_to_index(char src_file, char src_rank)
{
  return (src_file - 'a') + ((src_rank - '1') * 0x10);
}

std::string index_to_an_square(uint8_t index)
{
  return std::string() + (char)((index % 0x10) + 'a') +
         (char)((index / 0x10) + '1');
}

char index_to_an_file(uint8_t index)
{
  return (char)((index % 0x10) + 'a');
}

char index_to_an_rank(uint8_t index)
{
  return (char)((index / 0x10) + '1');
}

uint8_t char_to_piece(char piece)
{
  bool white = piece >= 'A' && piece <= 'Z';
  switch (std::toupper(piece))
  {
  case PAWN_CHAR:
    return white ? W_PAWN : B_PAWN;
  case ROOK_CHAR:
    return white ? W_ROOK : B_ROOK;
  case KNIGHT_CHAR:
    return white ? W_KNIGHT : B_KNIGHT;
  case BISHOP_CHAR:
    return white ? W_BISHOP : B_BISHOP;
  case QUEEN_CHAR:
    return white ? W_QUEEN : B_QUEEN;
  case KING_CHAR:
    return white ? W_KING : B_KING;
  default:
    return 0;
  }
}

char piece_to_char(uint8_t piece) { return PIECE_CHAR_MAP[piece]; }

const std::string piece_to_name(uint8_t piece)
{

  switch (piece & PIECE_MASK)
  {
  case PAWN:
  {
    return "PAWN";
  }
  case ROOK:
  {
    return "ROOK";
  }
  case KNIGHT:
  {
    return "KNIGHT";
  }
  case BISHOP:
  {
    return "BISHOP";
  }
  case QUEEN:
  {
    return "QUEEN";
  }
  case KING:
  {
    return "KING";
  }
  default:
  {
    return "INVALID_PIECE";
  }
  }
}

std::string piece_to_color_coded_char(uint8_t piece, bool highlight)
{
  // char chr = PIECE_CHAR_MAP[piece];
  // with color output, all of them can be made uppercase
  char chr = PIECE_CHAR_MAP[piece & PIECE_MASK];
  bool white = IS_WHITE_PIECE(piece);

  std::string white_color = "\u001b[38;5;231";
  std::string black_color = "\u001b[38;5;94";
  std::string highlight_code = ";48;5;";
  std::string highlight_color = "233";
  std::string escape = "\u001b[0m";

  // "\u001b[38;5;231;48;5;81m TEST \u001b[0m"
  // 81 is light blue
  // 244 is slate grey
  if (chr == '-')
  {
    if (highlight)
    {
      return std::string("\u001b[48;5;") + highlight_color + "m" + "-" + escape;
    }
    return std::string("-");
  }

  return (white ? white_color : black_color) +
         (highlight ? highlight_code + highlight_color : "") + "m" +
         chr + escape;
}

std::string piece_to_unicode_char(uint8_t piece)
{
  std::string unicode_str = UNICODE_PIECE_CHAR_MAP[piece];
  bool white = IS_WHITE_PIECE(piece);

  if (!unicode_str.compare("-"))
  {
    return std::string("-");
  }

  return (white ? std::string("\u001b[38;5;231m") : std::string("\u001b[38;5;94m")) + unicode_str + std::string("\u001b[0m");
}

char old_piece_to_char(uint8_t piece)
{
  bool white = IS_WHITE_PIECE(piece);
  char piece_char;
  switch (piece & PIECE_MASK)
  {
  case PAWN:
    piece_char = 'P';
    break;
  case ROOK:
    piece_char = 'R';
    break;
  case KNIGHT:
    piece_char = 'N';
    break;
  case BISHOP:
    piece_char = 'B';
    break;
  case QUEEN:
    piece_char = 'Q';
    break;
  case KING:
    piece_char = 'K';
    break;
  default:
    return '-';
  }
  return piece_char + (white ? 0x0 : 0x20);
}

void adjust_position(Position *position, uint8_t src_square,
                     uint8_t dest_square, uint8_t promotion_piece, uint8_t en_passant_square)
{
  assert(IS_VALID_SQUARE(src_square));
  assert(IS_VALID_SQUARE(dest_square));

  Color color = position->turn ? Color::WHITE : Color::BLACK;

  position->mailbox[dest_square] =
      promotion_piece ? promotion_piece : position->mailbox[src_square];

  if (position->en_passant_square &&
      position->en_passant_square == dest_square &&
      ((position->turn ? is_w_pawn : is_b_pawn)(position->mailbox[src_square])))
  {
    // If we are capturing en-passant there should never be a promotion piece
    assert(!promotion_piece);

    // Remove the pawn that is being captured
    uint8_t square_of_pawn_being_captured = BACKWARD_RANK(color, dest_square);
    assert(position->mailbox[square_of_pawn_being_captured] == position->turn
               ? B_PAWN
               : W_PAWN);
    position->mailbox[square_of_pawn_being_captured] = 0;
  }
  position->en_passant_square = en_passant_square;

  position->mailbox[src_square] = 0;
}

void perform_castle(Position *position, bool white, bool short_castle)
{
  assert(position->mailbox[white ? W_KING_SQUARE : B_KING_SQUARE] == white ? W_KING : B_KING);
  position->mailbox[white ? W_KING_SQUARE : B_KING_SQUARE] = 0;
  if (white)
  {
    if (short_castle)
    {
      position->mailbox[W_KING_ROOK_SQUARE] = 0;

      assert(position->mailbox[W_KING_SHORT_CASTLE_SQUARE] == 0);
      assert(position->mailbox[W_ROOK_SHORT_CASTLE_SQUARE] == 0);

      position->mailbox[W_KING_SHORT_CASTLE_SQUARE] = W_KING;
      position->mailbox[W_ROOK_SHORT_CASTLE_SQUARE] = W_ROOK;
    }
    else
    {
      position->mailbox[W_QUEEN_ROOK_SQUARE] = 0;

      assert(position->mailbox[W_KING_LONG_CASTLE_SQUARE] == 0);
      assert(position->mailbox[W_ROOK_LONG_CASTLE_SQUARE] == 0);

      position->mailbox[W_KING_LONG_CASTLE_SQUARE] = W_KING;
      position->mailbox[W_ROOK_LONG_CASTLE_SQUARE] = W_ROOK;
    }
  }
  else
  {
    if (short_castle)
    {
      position->mailbox[B_KING_ROOK_SQUARE] = 0;

      assert(position->mailbox[B_KING_SHORT_CASTLE_SQUARE] == 0);
      assert(position->mailbox[B_ROOK_SHORT_CASTLE_SQUARE] == 0);

      position->mailbox[B_KING_SHORT_CASTLE_SQUARE] = B_KING;
      position->mailbox[B_ROOK_SHORT_CASTLE_SQUARE] = B_ROOK;
    }
    else
    {
      position->mailbox[B_QUEEN_ROOK_SQUARE] = 0;

      assert(position->mailbox[B_KING_LONG_CASTLE_SQUARE] == 0);
      assert(position->mailbox[B_ROOK_LONG_CASTLE_SQUARE] == 0);

      position->mailbox[B_KING_LONG_CASTLE_SQUARE] = B_KING;
      position->mailbox[B_ROOK_LONG_CASTLE_SQUARE] = B_ROOK;
    }
  }
  position->en_passant_square = 0;
}

void print_position(Position *position)
{
  int i = 0x70; //0x70 is the top-left corner of the board
  while (1)
  {
    std::cout << piece_to_char(position->mailbox[i]) << " ";
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
    std::cout << piece_to_color_coded_char(position->mailbox[i], false) << " ";
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

void print_position_with_borders_highlight_squares(Position *position, uint8_t src_square, uint8_t dest_square)
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
      std::cout << piece_to_color_coded_char(position->mailbox[i], true) << " ";
    }
    else if (i == dest_square)
    {
      std::cout << piece_to_color_coded_char(position->mailbox[i], true) << " ";
    }
    else
    {
      std::cout << piece_to_color_coded_char(position->mailbox[i], false) << " ";
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

/*
  Returns the square with the (white ? white : black) king. Returns 127 otherwise.
*/
uint8_t find_king(Position *position, bool white)
{
  uint8_t target = white ? W_KING : B_KING;
  for (int rank = 0; rank < 8; rank++)
  {
    for (int file = 0; file < 8; file++)
    {
      uint8_t square = (16 * rank) + file;
      if (position->mailbox[square] == target)
      {
        return square;
      }
    }
  }
  return INVALID_SQUARE;
}

// returns a vector containing all the squares of enemy pieces that diagonally attack the target square.
// TODO define separate types for squares and pieces, because uint8_t everywhere is confusing.
std::vector<uint8_t> check_diagonals(Position *position, uint8_t target_square, bool color_of_attackers)
{
  std::vector<uint8_t> squares;
  //look for bishops/queens attacking square on diagonals
  for (int i = 0; i < 4; i++)
  {
    uint8_t square = check_line(
        position,
        target_square,
        bishop_offsets[i],
        color_of_attackers ? &white_attacks_diagonally : &black_attacks_diagonally);
    if (IS_VALID_SQUARE(square))
    {
      squares.push_back(square);
    }
  }

  return squares;
}

std::vector<uint8_t> find_attacking_bishops(Position *position, uint8_t target_square, bool color_of_attackers)
{
  std::vector<uint8_t> squares;
  //look for bishops attacking square on diagonals
  for (int i = 0; i < 4; i++)
  {
    uint8_t square = check_line(
        position,
        target_square,
        bishop_offsets[i],
        color_of_attackers ? &is_w_bishop : &is_b_bishop);
    if (IS_VALID_SQUARE(square))
    {
      squares.push_back(square);
    }
  }

  return squares;
}

std::vector<uint8_t> find_attacking_rooks(Position *position, uint8_t target_square, bool color_of_attackers)
{
  std::vector<uint8_t> squares;
  //look for rooks attacking square on diagonals
  for (int i = 0; i < 4; i++)
  {
    uint8_t square = check_line(position, target_square, rook_offsets[i], color_of_attackers ? &is_w_rook : &is_b_rook);
    if (IS_VALID_SQUARE(square))
    {
      squares.push_back(square);
    }
  }

  return squares;
}

std::vector<uint8_t> find_attacking_queens(Position *position, uint8_t target_square, bool color_of_attackers)
{
  std::vector<uint8_t> squares;
  //look for queens attacking square on diagonals
  for (int i = 0; i < 4; i++)
  {
    uint8_t square = check_line(
        position,
        target_square,
        bishop_offsets[i],
        color_of_attackers ? &is_w_queen : &is_b_queen);
    if (IS_VALID_SQUARE(square))
    {
      squares.push_back(square);
    }
    square = check_line(
        position,
        target_square,
        rook_offsets[i],
        color_of_attackers ? &is_w_queen : &is_b_queen);
    if (IS_VALID_SQUARE(square))
    {
      squares.push_back(square);
    }
  }

  return squares;
}

// returns a vector containing all the squares of enemy pieces that diagonally attack the target square.
// TODO define separate types for squares and pieces, because uint8_t everywhere is confusing.
std::vector<uint8_t> check_files_ranks(Position *position, uint8_t target_square, bool color_of_attackers)
{
  std::vector<uint8_t> squares;
  //look for rooks/queens attacking square on files and ranks
  for (int i = 0; i < 4; i++)
  {
    uint8_t square = check_line(
        position,
        target_square,
        rook_offsets[i],
        color_of_attackers ? &white_attacks_files_ranks : &black_attacks_files_ranks);
    if (IS_VALID_SQUARE(square))
    {
      squares.push_back(square);
    }
  }

  return squares;
}

// returns the square of the first piece that attacks the target along the line.
uint8_t check_line(Position *position, uint8_t target, int offset, bool (*piece_type_function)(uint8_t))
{
  for (uint8_t candidate = target + offset; IS_VALID_SQUARE(candidate); candidate += offset)
  {
    uint8_t piece = position->mailbox[candidate];
    // square is empty so we need to keep looking.
    if (piece == 0)
    {
      continue;
    }

    // square contains an enemy piece that can attack along this line
    if (piece_type_function(position->mailbox[candidate]))
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

bool check_diagonal_or_file_or_rank(Position *position, uint8_t king_square, int offset, uint8_t target1, uint8_t target2)
{
  for (uint8_t candidate = king_square + offset; IS_VALID_SQUARE(candidate); candidate += offset)
  {
    uint8_t piece = position->mailbox[candidate];
    // square is empty so we need to keep looking.
    if (piece == 0)
    {
      continue;
    }

    // square contains either the enemy bishop or queen.
    if (position->mailbox[candidate] == target1 || position->mailbox[candidate] == target2)
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

// king cannot be attacked by an enemy piece (unless it is the king's player's turn to move)
bool legal_position(Position *position, bool whites_turn)
{
  Color enemy_color = whites_turn ? Color::BLACK : Color::WHITE;

  // If it is white's turn to move, we have to make sure the black king is not in check (and vice-versa)
  uint8_t king_square = find_king(position, whites_turn);

  // look for pawns attacking king
  uint8_t target = whites_turn ? B_PAWN : W_PAWN;
  uint8_t candidate = PREV_FILE(BACKWARD_RANK(enemy_color, king_square));
  if (IS_VALID_SQUARE(candidate) && position->mailbox[candidate] == target)
  {
    std::cout << "pawn attacking king first condition." << std::endl;
    return false;
  }
  candidate = NEXT_FILE(BACKWARD_RANK(enemy_color, king_square));
  if (IS_VALID_SQUARE(candidate) && position->mailbox[candidate] == target)
  {
    std::cout << "illegal_position: found pawn attacking king second condition." << std::endl;
    return false;
  }

  // look for knights attacking king
  std::vector<uint8_t> knights;
  target = whites_turn ? B_KNIGHT : W_KNIGHT;
  for (auto it = knight_move_offsets.begin(); it != knight_move_offsets.end(); it++)
  {
    candidate = *it + king_square;
    if (IS_INVALID_SQUARE(candidate))
    {
      continue;
    }
    if (position->mailbox[candidate] == target)
    {
      std::cout << "illegal_position: found knight attacking king" << std::endl;
      return false;
    }
  }

  if (!check_diagonals(position, king_square, !whites_turn).empty())
  {
    std::cout << "illegal_position: found diagonal attacking king" << std::endl;
    return false;
  }

  //look for bishops/queens attacking king on diagonals
  if (!check_files_ranks(position, king_square, !whites_turn).empty())
  {
    std::cout << "illegal_position: found files/ranks attacking king" << std::endl;
    return false;
  }

  // look for kings next to each other. it doesnt matter whose turn it is when this happens, its always illegal.
  target = whites_turn ? B_KING : W_KING;
  for (auto it = directions_vector.begin(); it != directions_vector.end(); it++)
  {
    candidate = king_square + direction_offset(*it);
    if (IS_VALID_SQUARE(candidate) && position->mailbox[candidate] == target)
    {
      std::cout << "candidate: " << candidate << "position->mailbox[candidate]:" << position->mailbox[candidate] << std::endl;
      std::cout << "illegal_position: found kings?? attacking king" << std::endl;
      return false;
    }
  }

  return true;
}
