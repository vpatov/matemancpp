#ifndef __POSITION_H__
#define __POSITION_H__

#include "squares.hpp"
#include <assert.h>
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

#define VOID_PIECE 0

#define PAWN_CHAR 'P'
#define ROOK_CHAR 'R'
#define KNIGHT_CHAR 'N'
#define BISHOP_CHAR 'B'
#define QUEEN_CHAR 'Q'
#define KING_CHAR 'K'

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

#define B_PAWN (PAWN | BLACK_PIECE_MASK)
#define B_ROOK (ROOK | BLACK_PIECE_MASK)
#define B_KNIGHT (KNIGHT | BLACK_PIECE_MASK)
#define B_BISHOP (BISHOP | BLACK_PIECE_MASK)
#define B_QUEEN (QUEEN | BLACK_PIECE_MASK)
#define B_KING (KING | BLACK_PIECE_MASK)

const char PIECE_CHAR_MAP[24] = {'-', 'P', 'R', 'N', 'B', 'Q', 'K', 0,
                                 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 'p', 'r', 'n', 'b', 'q', 'k', 0};

const std::string UNICODE_PIECE_CHAR_MAP[24] = {"-", "♙", "♖", "♘", "♗", "♕", "♔", " ",
                                                " ", " ", " ", " ", " ", " ", " ", " ",
                                                " ", "♟︎", "♜", "♞", "♝", "♛", "♚", " "};

#define MAX_PIECE B_KING

#define INVALID_SQUARE 127
#define IS_INVALID_SQUARE(sq) (sq & 0x88)
#define IS_VALID_SQUARE(sq) (!(IS_INVALID_SQUARE(sq)))

#define IS_PIECE(piece) (piece & PIECE_MASK)
#define IS_BLACK_PIECE(piece) (piece & BLACK_PIECE_MASK)
#define IS_WHITE_PIECE(piece) (piece && !(IS_BLACK_PIECE(piece)))

struct Position
{
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

class PositionC
{
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
uint8_t an_square_to_index(char src_file, char src_rank);
uint8_t char_to_piece(char piece);
char piece_to_char(uint8_t piece);
std::string piece_to_color_coded_char(uint8_t piece);
std::string piece_to_unicode_char(uint8_t piece);
std::string index_to_an_square(uint8_t index);
char index_to_an_file(uint8_t index);
char index_to_an_rank(uint8_t index);
void print_position(Position *position);
void print_position_with_borders(Position *position);
std::shared_ptr<Position> starting_position();
void populate_starting_position(Position *position);

void adjust_position(Position *position, uint8_t src_square,
                     uint8_t dest_square, uint8_t promotion_piece, uint8_t en_passant_square);

uint8_t find_king(Position *position, bool white);

void perform_castle(Position *position, bool white, bool short_castle);

// king cannot be attacked by an enemy piece (unless it is the king's player's turn to move)
bool legal_position(Position *position, bool whites_turn);

uint8_t check_line(Position *position, uint8_t target, int offset, bool (*piece_type_function)(uint8_t));
std::vector<uint8_t> check_files_ranks(Position *position, uint8_t target_square, bool color_of_attackers);
std::vector<uint8_t> check_diagonals(Position *position, uint8_t target_square, bool color_of_attackers);
std::vector<uint8_t> find_attacking_bishops(Position *position, uint8_t target_square, bool color_of_attackers);
std::vector<uint8_t> find_attacking_rooks(Position *position, uint8_t target_square, bool color_of_attackers);
std::vector<uint8_t> find_attacking_queens(Position *position, uint8_t target_square, bool color_of_attackers);

#endif