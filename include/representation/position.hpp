#pragma once

#include "pieces.hpp"
#include "squares.hpp"
#include "color.hpp"
#include "util.hpp"
#include "representation/move.hpp"
#include <regex>
#include <assert.h>
#include <cstdint>
#include <memory>
#include <string>
#include <iostream>

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

  In hex

a   b   c   d   e   f   g   h

70  71  72  73  74  75  76  77  |   78  79  7a  7b  7c  7d  7e  7f
60  61  62  63  64  65  66  67  |   68  69  6a  6b  6c  6d  6e  6f
50  51  52  53  54  55  56  57  |   58  59  5a  5b  5c  5d  5e  5f
40  41  42  43  44  45  46  47  |   48  49  4a  4b  4c  4d  4e  4f
30  31  32  33  34  35  36  37  |   38  39  3a  3b  3c  3d  3e  3f
20  21  22  23  24  25  26  27  |   28  29  2a  2b  2c  2d  2e  2f
10  11  12  13  14  15  16  17  |   18  19  1a  1b  1c  1d  1e  1f
00  01  02  03  04  05  06  07  |   08  09  0a  0b  0c  0d  0e  0f

a   b   c   d   e   f   g   h
*/

enum Direction
{
  UP,
  DOWN,
  LEFT,
  RIGHT,
  UPLEFT,
  UPRIGHT,
  DOWNLEFT,
  DOWNRIGHT
};

const std::vector<Direction> directions_vector = {
    Direction::UP,
    Direction::DOWN,
    Direction::LEFT,
    Direction::RIGHT,
    Direction::UPLEFT,
    Direction::UPRIGHT,
    Direction::DOWNLEFT,
    Direction::DOWNRIGHT};

const int bishop_offsets[4] = {15, 17, -15, -17};
const int rook_offsets[4] = {16, 1, -16, -1};

#define PAWNC(C) (is_white(C) ? W_PAWN : B_PAWN)
#define ROOKC(C) (is_white(C) ? W_ROOK : B_ROOK)
#define KNIGHTC(C) (is_white(C) ? W_KNIGHT : B_KNIGHT)
#define BISHOPC(C) (is_white(C) ? W_BISHOP : B_BISHOP)
#define QUEENC(C) (is_white(C) ? W_QUEEN : B_QUEEN)
#define KINGC(C) (is_white(C) ? W_KING : B_KING)

#define KING_ROOK_SQUARE_C(C) \
  (is_white(C) ? W_KING_ROOK_SQUARE : B_KING_ROOK_SQUARE)
#define QUEEN_ROOK_SQUARE_C(C) \
  (is_white(C) ? W_QUEEN_ROOK_SQUARE : B_QUEEN_ROOK_SQUARE)
#define KING_SHORT_CASTLE_SQUARE_C(C) \
  (is_white(C) ? W_KING_SHORT_CASTLE_SQUARE : B_KING_SHORT_CASTLE_SQUARE)
#define KING_LONG_CASTLE_SQUARE_C(C) \
  (is_white(C) ? W_KING_LONG_CASTLE_SQUARE : B_KING_LONG_CASTLE_SQUARE)

#define ROOK_SHORT_CASTLE_SQUARE_C(C) \
  (is_white(C) ? W_ROOK_SHORT_CASTLE_SQUARE : B_ROOK_SHORT_CASTLE_SQUARE)
#define ROOK_LONG_CASTLE_SQUARE_C(C) \
  (is_white(C) ? W_ROOK_LONG_CASTLE_SQUARE : B_ROOK_LONG_CASTLE_SQUARE)

#define KING_KNIGHT_SQUARE_C(C) \
  (is_white(C) ? W_KING_KNIGHT_SQUARE : B_KING_KNIGHT_SQUARE)
#define KING_BISHOP_SQUARE_C(C) \
  (is_white(C) ? W_KING_BISHOP_SQUARE : B_KING_BISHOP_SQUARE)

#define QUEEN_SQUARE_C(C) \
  (is_white(C) ? W_QUEEN_SQUARE : B_QUEEN_SQUARE)
#define QUEEN_KNIGHT_SQUARE_C(C) \
  (is_white(C) ? W_QUEEN_KNIGHT_SQUARE : B_QUEEN_KNIGHT_SQUARE)
#define QUEEN_BISHOP_SQUARE_C(C) \
  (is_white(C) ? W_QUEEN_BISHOP_SQUARE : B_QUEEN_BISHOP_SQUARE)

#define KING_SQUARE_C(C) \
  (is_white(C) ? W_KING_SQUARE : B_KING_SQUARE)

#define RANK_OFFSET 16
#define FILE_OFFSET 1

#define NEXT_RANK(sq) (static_cast<uint8_t>(sq + RANK_OFFSET))
#define PREV_RANK(sq) (static_cast<uint8_t>(sq - RANK_OFFSET))

#define PREV_FILE(sq) (static_cast<uint8_t>(sq - FILE_OFFSET))
#define NEXT_FILE(sq) (static_cast<uint8_t>(sq + FILE_OFFSET))

#define RANKC_TO_RANK(rankc) (rankc - '1')
#define FILEC_TO_FILE(filec) (filec - 'a')

// Returns the rank that is forward relative to the player
#define FORWARD_RANK(C, square) \
  (is_white(C) ? NEXT_RANK(square) : PREV_RANK(square))

#define BACKWARD_RANK(C, square) \
  (is_white(C) ? PREV_RANK(square) : NEXT_RANK(square))

#define IN_FIRST_RANK(square) \
  square <= H1_SQ
#define IN_EIGHTH_RANK(square) \
  square >= A8_SQ
#define IN_LAST_PAWN_RANK_C(C, square) \
  (is_white(C) ? IN_EIGHTH_RANK(square) : IN_FIRST_RANK(square))

#define IN_SECOND_RANK(sq) (sq >= 16 && sq <= 23)
#define IN_SEVENTH_RANK(sq) (sq >= 96 && sq <= 103)
#define IN_START_PAWN_RANK(C, square) \
  (is_white(C) ? IN_SECOND_RANK(square) : IN_SEVENTH_RANK(square))

#define IS_YOUR_PIECE(C, piece) \
  (is_white(C) ? is_white_piece(piece) : is_black_piece(piece))
#define IS_OPPONENT_PIECE(C, piece) \
  (is_white(C) ? is_black_piece(piece) : is_white_piece(piece))

#define ATTACKS_DIAGONALLY(piece) \
  (((piece & PIECE_MASK) == BISHOP) || (piece & PIECE_MASK) == QUEEN)

#define ATTACKS_FILES_RANKS(piece) \
  (((piece & PIECE_MASK) == ROOK) || (piece & PIECE_MASK) == QUEEN)

constexpr uint8_t direction_offset(Direction D)
{
  switch (D)
  {
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

const std::vector<int> knight_move_offsets = {
    RANK_OFFSET + RANK_OFFSET + FILE_OFFSET,
    RANK_OFFSET + RANK_OFFSET - FILE_OFFSET,
    -RANK_OFFSET - RANK_OFFSET + FILE_OFFSET,
    -RANK_OFFSET - RANK_OFFSET - FILE_OFFSET,
    FILE_OFFSET + FILE_OFFSET + RANK_OFFSET,
    FILE_OFFSET + FILE_OFFSET - RANK_OFFSET,
    -FILE_OFFSET - FILE_OFFSET + RANK_OFFSET,
    -FILE_OFFSET - FILE_OFFSET - RANK_OFFSET};

#define STEP_DIRECTION(D, square) (direction_offset(D) + square)

struct Position
{
public:
  piece_t m_mailbox[128]; // x88 mailbox flat array
  bool m_whites_turn;     // true if white's turn
  bool m_white_kingside_castle;
  bool m_white_queenside_castle;
  bool m_black_kingside_castle;
  bool m_black_queenside_castle;
  int m_plies;
  int m_moves;
  square_t m_en_passant_square = INVALID_SQUARE;
  uint32_t castling_move(std::smatch &matches, bool white);
  uint32_t non_castling_move(
      char piece_char, char src_file, char src_rank, char capture,
      char dest_file, char dest_rank, char promotion_piece,
      char check_or_mate);
  void perform_castle(bool white, bool short_castle);
  void print_with_borders_highlight_squares(square_t src_square, square_t dest_square);
  std::string pretty_string();

  square_t find_king();
  uint16_t get_src_square_pawn_move(char capture, char src_file, square_t dest_square, uint8_t dest_rank);
  square_t get_src_square_minmaj_piece_move(
      char piece_char, uint8_t src_file, uint8_t src_rank,
      square_t dest_square, square_t en_passant_square);
  void assert_correct_player_turn(square_t src_square, square_t dest_square);
  void adjust_position(square_t src_square, square_t dest_square, piece_t promotion_piece, square_t en_passant_square);
  bool legal_position();
  void advance_position(Move move);
  void advance_position(square_t src_square, square_t dst_square, uint8_t promotion_piece);
  bool is_move_legal(square_t src_square, square_t dst_square);

  bool operator==(const Position &rhs) const
  {

    for (square_t sq = 0; sq <= H8_SQ; sq++)
    {
      if (is_valid_square(sq) && m_mailbox[sq] != rhs.m_mailbox[sq])
      {
        return false;
      }
    }
    // TODO compare m_moves and or m_plies once you've figured out their semantics
    if (
        m_whites_turn != rhs.m_whites_turn ||
        m_white_kingside_castle != rhs.m_white_kingside_castle ||
        m_white_queenside_castle != rhs.m_white_queenside_castle ||
        m_black_kingside_castle != rhs.m_black_kingside_castle ||
        m_black_queenside_castle != rhs.m_black_queenside_castle ||
        m_en_passant_square != rhs.m_en_passant_square)
    {
      return false;
    }

    return true;
  }
};

// TODO put some of these methods in a different file, cut dead code, refactor similarities

void print_position(Position *position);
void print_position_with_borders(Position *position);
void print_position_with_borders_highlight_squares(Position *position, square_t src_square, square_t dest_square);
std::shared_ptr<Position> starting_position();
void populate_starting_position(Position *position);
std::unique_ptr<Position> generate_starting_position();

uint8_t find_king(Position *position, bool white);

void perform_castle(Position *position, bool white, bool short_castle);

// king cannot be attacked by an enemy piece (unless it is the king's player's turn to move)
bool legal_position(Position *position, bool whites_turn);

square_t check_line(Position *position, square_t target, int offset, bool (*piece_type_function)(piece_t));
std::vector<uint8_t> check_files_ranks(Position *position, square_t target_square, bool color_of_attackers);
std::vector<uint8_t> check_diagonals(Position *position, square_t target_square, bool color_of_attackers);
std::vector<uint8_t> find_attacking_knights(Position *position, square_t target_square, bool color_of_attackers);
std::vector<uint8_t> find_attacking_bishops(Position *position, square_t target_square, bool color_of_attackers);
std::vector<uint8_t> find_attacking_rooks(Position *position, square_t target_square, bool color_of_attackers);
std::vector<uint8_t> find_attacking_queens(Position *position, square_t target_square, bool color_of_attackers);
