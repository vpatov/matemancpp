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

// can this be one 64-bit word??????
struct PositionAdjustment
{
  square_t src_square;
  square_t dst_square;
  piece_t captured_piece;
  square_t pawn_captured_en_passant_square;
  piece_t moving_piece;
  square_t old_en_passant_square;
  uint8_t old_castling_rights;
  uint8_t castled; // if this move was a castle, will be non zero. 1 -> short castle, 2-> long castle
};

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
  PositionAdjustment advance_position(Move move);
  PositionAdjustment advance_position(square_t src_square, square_t dst_square, uint8_t promotion_piece);
  bool is_move_legal(square_t src_square, square_t dst_square);

  void undo_adjustment(PositionAdjustment a)
  {
    // square_t old_en_passant_square;
    // uint8_t old_castling_rights;
    // uint8_t castled;

    // color of the pieces (if any) that were captured, that we are restoring
    Color captured_color = m_whites_turn ? Color::WHITE : Color::BLACK;
    // color of the player that made the move we are undoing
    Color move_maker_color = m_whites_turn ? Color::BLACK : Color::WHITE;

    m_mailbox[a.src_square] = a.moving_piece;
    m_mailbox[a.dst_square] = a.captured_piece;
    if (is_valid_square(a.pawn_captured_en_passant_square))
    {
      m_mailbox[a.pawn_captured_en_passant_square] = PAWN_C(captured_color);
    }
    m_en_passant_square = a.old_en_passant_square;

    // restore castling rights
    m_white_kingside_castle = a.old_castling_rights & (1);
    m_white_queenside_castle = a.old_castling_rights & (1 << 1);
    m_black_kingside_castle = a.old_castling_rights & (1 << 2);
    m_black_queenside_castle = a.old_castling_rights & (1 << 3);

    // undo castling move if done
    switch (a.castled)
    {
    case 0:
      break;
    // short castle
    case 1:
    {
      m_mailbox[KING_SHORT_CASTLE_SQUARE_C(move_maker_color)] = VOID_PIECE;
      m_mailbox[ROOK_SHORT_CASTLE_SQUARE_C(move_maker_color)] = VOID_PIECE;
      m_mailbox[KING_ROOK_SQUARE_C(move_maker_color)] = ROOK_C(move_maker_color);
      m_mailbox[KING_SQUARE_C(move_maker_color)] = KING_C(move_maker_color);
      (move_maker_color == Color::WHITE ? m_white_kingside_castle : m_black_kingside_castle) = true;
      break;
    }
    // long castle
    case 2:
    {
      m_mailbox[KING_LONG_CASTLE_SQUARE_C(move_maker_color)] = VOID_PIECE;
      m_mailbox[ROOK_LONG_CASTLE_SQUARE_C(move_maker_color)] = VOID_PIECE;
      m_mailbox[QUEEN_ROOK_SQUARE_C(move_maker_color)] = ROOK_C(move_maker_color);
      m_mailbox[KING_SQUARE_C(move_maker_color)] = KING_C(move_maker_color);
      (move_maker_color == Color::WHITE ? m_white_queenside_castle : m_black_queenside_castle) = true;
      break;
    }
    default:
      __builtin_unreachable();
    }

    m_plies--;
    if (move_maker_color == Color::BLACK)
    {
      m_moves--;
    }
    m_whites_turn = !m_whites_turn;
  }

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
std::vector<square_t> check_files_ranks(Position *position, square_t target_square, bool color_of_attackers);
std::vector<square_t> check_diagonals(Position *position, square_t target_square, bool color_of_attackers);
std::vector<square_t> find_attacking_knights(Position *position, square_t target_square, bool color_of_attackers);
std::vector<square_t> find_attacking_bishops(Position *position, square_t target_square, bool color_of_attackers);
std::vector<square_t> find_attacking_rooks(Position *position, square_t target_square, bool color_of_attackers);
std::vector<square_t> find_attacking_queens(Position *position, square_t target_square, bool color_of_attackers);
