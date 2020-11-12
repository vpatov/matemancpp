#include "move_generation.hpp"
#include "position.hpp"
#include <assert.h>
#include <cstdint>
#include <iostream>
#include <vector>

/** Pseudolegal moves don't take check into account. */
std::vector<uint8_t>
generate_pseudolegal_wpawn_moves(std::shared_ptr<Position> position,
                                 uint8_t square) {
  assert(VALID_SQUARE(square));
  assert(position->mailbox[square] == W_PAWN);

  uint8_t candidate;
  std::vector<uint8_t> moves;

  // check square in front
  candidate = NEXT_RANK(square);
  if (VALID_SQUARE(candidate) && position->mailbox[candidate] == VOID_PIECE) {
    moves.push_back(candidate);

    // if square in front is empty, and we're on second rank, we can move two
    // squares
    candidate = NEXT_RANK(candidate);
    if (IN_SECOND_RANK(square) && VALID_SQUARE(candidate) &&
        position->mailbox[candidate] == VOID_PIECE) {
      moves.push_back(candidate);
    }
  }

  // check diagonals for capture
  candidate = PREVIOUS_FILE(NEXT_RANK(square));
  if (VALID_SQUARE(candidate) && IS_BLACK_PIECE(position->mailbox[candidate])) {
    moves.push_back(candidate);
  }

  candidate = NEXT_FILE(NEXT_RANK(square));
  if (VALID_SQUARE(candidate) && IS_BLACK_PIECE(position->mailbox[candidate])) {
    moves.push_back(candidate);
  }

  return moves;
}