#include "representation/fen.hpp"
#include "representation/position.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

// Example FEN string (starting position):
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
std::shared_ptr<Position> fen_to_position(std::string fen)
{
  uint8_t index = 0x70;
  auto position = std::make_shared<Position>();

  bool expect_rank_separator = false;
  auto it = fen.begin();

  for (; it != fen.end(); it++)
  {
    char c = *it;
    if (expect_rank_separator)
    {
      if (c != '/')
      {
        std::cerr << "Encountered invalid FEN string" << std::endl;
        return NULL;
      }
      else
      {
        expect_rank_separator = false;
        continue;
      }
    }
    // if the current character represents the amount of empty spaces
    if (c > '0' && c <= '8')
    {
      index += c - '0';
    }
    // otherwise we except a piece symbol
    else
    {
      // the bool and mask below are such that we can use the
      // same code for the white and black pieces
      bool white = c >= 'A' && c <= 'Z';
      uint8_t mask = white ? 0 : BLACK_PIECE_MASK;
      c += white ? 0x20 : 0;

      switch (c)
      {
      case 'r':
        position->m_mailbox[index] = ROOK | mask;
        break;
      case 'n':
        position->m_mailbox[index] = KNIGHT | mask;
        break;
      case 'b':
        position->m_mailbox[index] = BISHOP | mask;
        break;
      case 'q':
        position->m_mailbox[index] = QUEEN | mask;
        break;
      case 'k':
        position->m_mailbox[index] = KING | mask;
        break;
      case 'p':
        position->m_mailbox[index] = PAWN | mask;
        break;
      default:
        std::cout << "Encountered an unexpected character: " << c << std::endl;
        return NULL;
      }

      index++;
    }

    if (index == 0x8)
    {
      it += 2;
      break;
    }

    if (index & 0x88)
    {
      expect_rank_separator = true;
      index -= 0x18;
    }
  }

  std::vector<std::string> fen_parts;
  boost::algorithm::split(fen_parts, fen.substr(std::distance(fen.begin(), it)),
                          boost::algorithm::is_any_of("\t "),
                          boost::token_compress_on);

  position->m_whites_turn = fen_parts.at(0).at(0) == 'w';
  for (auto it = fen_parts.at(1).begin(); it != fen_parts.at(1).end(); it++)
  {
    switch (*it)
    {
    case 'K':
      position->m_white_kingside_castle = true;
      break;
    case 'Q':
      position->m_white_queenside_castle = true;
      break;
    case 'k':
      position->m_black_kingside_castle = true;
      break;
    case 'q':
      position->m_black_queenside_castle = true;
      break;
    case '-':
      break;
    default:
      std::cerr << "Invalid FEN string." << std::endl;
      return NULL;
    }
  }

  if (fen_parts.at(2).size() == 2)
  {

    square_t square = an_square_to_index(fen_parts.at(2));
    assert(is_valid_square(square));
    position->m_en_passant_square = square;
  }

  return position;
}
