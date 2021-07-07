#include "position.hpp"
#include "move_generation.hpp"
#include <iostream>
#include <memory>

std::shared_ptr<Position> starting_position() {
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
  for (int i = 0x10; i < 0x18; i++) {
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
  for (int i = 0x60; i < 0x68; i++) {
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

void populate_starting_position(Position *position) {
  /** White pieces*/
  position->mailbox[0x0] = W_ROOK;
  position->mailbox[0x1] = W_KNIGHT;
  position->mailbox[0x2] = W_BISHOP;
  position->mailbox[0x3] = W_QUEEN;
  position->mailbox[0x4] = W_KING;
  position->mailbox[0x5] = W_BISHOP;
  position->mailbox[0x6] = W_KNIGHT;
  position->mailbox[0x7] = W_ROOK;
  for (int i = 0x10; i < 0x18; i++) {
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
  for (int i = 0x60; i < 0x68; i++) {
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

uint8_t an_square_to_index(std::string square) {
  return (square.at(0) - 'a') + ((square.at(1) - '1') * 0x10);
}

uint8_t an_square_to_index(char src_file, char src_rank) {
  return (src_file - 'a') + ((src_rank - '1') * 0x10);
}

std::string index_to_an_square(uint8_t index) {
  return std::string() + (char)((index % 0x10) + 'a') +
         (char)((index / 0x10) + '1');
}

uint8_t char_to_piece(char piece) {
  bool white = piece >= 'A' && piece <= 'Z';
  switch (std::toupper(piece)) {
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

char old_piece_to_char(uint8_t piece) {
  bool white = piece & BLACK_PIECE_MASK;
  char piece_char;
  switch (piece & PIECE_MASK) {
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
                     uint8_t dest_square, uint8_t promotion_piece) {
  assert(VALID_SQUARE(src_square));
  assert(VALID_SQUARE(dest_square));

  position->mailbox[dest_square] =
      promotion_piece ? promotion_piece : position->mailbox[src_square];

  if (position->en_passant_square &&
      position->en_passant_square == dest_square) {
    // If we are capturing en-passant there should never be a promotion piece
    assert(!promotion_piece);

    Color color = position->turn ? Color::WHITE : Color::BLACK;
    // Remove the pawn that is being captured
    uint8_t square_of_pawn_being_captured = BACKWARD_RANK(color, dest_square);
    assert(position->mailbox[square_of_pawn_being_captured] == position->turn
               ? B_PAWN
               : W_PAWN);
    position->mailbox[square_of_pawn_being_captured] = 0;
  }

  position->mailbox[src_square] = 0;
}

void print_position(Position *position) {
  int i = 0x70;
  while (1) {
    std::cout << piece_to_char(position->mailbox[i]) << " ";
    i++;
    if (i == 8) {
      std::cout << std::endl << std::endl;
      return;
    }
    if (i & 0x88) {
      i -= 0x18;
      std::cout << std::endl;
    }
  }
}