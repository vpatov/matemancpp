#include "position.hpp"
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

char piece_to_char(uint8_t piece) {
  bool white = piece & BLACK_PIECE_MASK;
  switch (piece & PIECE_MASK) {
  case PAWN:
    return 'P' + (white ? 0x20 : 0);
  case ROOK:
    return 'R' + (white ? 0x20 : 0);
  case KNIGHT:
    return 'N' + (white ? 0x20 : 0);
  case BISHOP:
    return 'B' + (white ? 0x20 : 0);
  case QUEEN:
    return 'Q' + (white ? 0x20 : 0);
  case KING:
    return 'K' + (white ? 0x20 : 0);
  default:
    return '-';
  }
}

void print_position(std::shared_ptr<Position> position) {
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