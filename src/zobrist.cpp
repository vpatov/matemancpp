#include "zobrist.hpp"
#include "position.hpp"
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
// (need to have the same initialziation across boots)
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

uint64_t zobrist_hash(std::shared_ptr<Position> position)
{
  uint64_t hash = 0;
  uint8_t square = 0;

  while (square < 120)
  {
    if (IS_INVALID_SQUARE(square))
    {
      square += 8;
    }
    uint8_t piece = position->mailbox[square];
    if (IS_PIECE(piece))
    {
      assert(piece <= MAX_PIECE);
      hash ^= zobrist_table[square][piece];
    }
    square++;
  }
  return hash;
}