#include "tablebase/zobrist.hpp"
#include <assert.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>

uint64_t random_bitstring()
{
  std::random_device rd;
  std::mt19937_64 e2(rd());
  std::uniform_int_distribution<uint64_t> dist(std::llround(std::pow(2, 61)),
                                               std::llround(std::pow(2, 62)));
  return dist(e2);
}

// Print out a zobrist intialization, so that can it be pasted into the source
// (need to have the same initialization across program starts otherwise tablebase has to be reconstructed each time)
void print_zobrist()
{
  std::cout << "{" << std::endl;
  for (int square = 0; square < MAILBOX_LENGTH; square++)
  {
    std::cout << "{" << std::endl;

    for (int piece = 0; piece < MAX_PIECE; piece++)
    {
      std::cout << random_bitstring();
      if (piece != MAX_PIECE - 1)
      {
        std::cout << ", ";
      }
    }
    std::cout << ((square == MAILBOX_LENGTH - 1) ? "}" : "},") << std::endl;
  }
  std::cout << "}" << std::endl;
}

z_hash_t zobrist_hash(Position *position)
{
  z_hash_t hash = 0;
  uint8_t square = 0;

  while (square < 120)
  {
    if (is_invalid_square(square))
    {
      square += 8;
    }
    uint8_t piece = position->m_mailbox[square];
    if (is_piece(piece))
    {
      assert(piece <= MAX_PIECE);
      hash ^= zobrist_piece_table[square][piece];
    }
    square++;
  }
  if (position->m_white_kingside_castle)
  {
    hash ^= zobrist_castling_rights_table[0];
  }
  if (position->m_white_queenside_castle)
  {
    hash ^= zobrist_castling_rights_table[1];
  }
  if (position->m_black_kingside_castle)
  {
    hash ^= zobrist_castling_rights_table[2];
  }
  if (position->m_black_queenside_castle)
  {
    hash ^= zobrist_castling_rights_table[3];
  }

  hash ^= zobrist_turn_table[(position->m_whites_turn) ? 1 : 0];

  if (is_valid_square(position->m_en_passant_square))
  {
    hash ^= zobrist_en_passant_square_table[position->m_en_passant_square];
  }

  return hash;
}