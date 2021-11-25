#include "representation/fen.hpp"
#include "representation/position.hpp"
#include "representation/notation.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

// Example FEN string (starting position):
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
std::shared_ptr<Position> fen_to_position(std::string fen)
{
  square_t square = 0x70;
  auto position = std::make_shared<Position>();

  bool expect_rank_separator = false;
  auto it = fen.begin();

  // pieces on the board
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
      square += c - '0';
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
        position->m_mailbox[square] = ROOK | mask;
        break;
      case 'n':
        position->m_mailbox[square] = KNIGHT | mask;
        break;
      case 'b':
        position->m_mailbox[square] = BISHOP | mask;
        break;
      case 'q':
        position->m_mailbox[square] = QUEEN | mask;
        break;
      case 'k':
        position->m_mailbox[square] = KING | mask;
        break;
      case 'p':
        position->m_mailbox[square] = PAWN | mask;
        break;
      default:
        std::cout << "Encountered an unexpected character: " << c << std::endl;
        return NULL;
      }

      square++;
    }

    if (square == 0x8)
    {
      it += 2;
      break;
    }

    if (is_invalid_square(square))
    {
      expect_rank_separator = true;
      square -= 0x18;
    }
  }

  std::vector<std::string> fen_parts;
  boost::algorithm::split(fen_parts, fen.substr(std::distance(fen.begin(), it)),
                          boost::algorithm::is_any_of("\t "),
                          boost::token_compress_on);

  // whose turn
  position->m_whites_turn = fen_parts.at(0).at(0) == 'w';

  // castling rights
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
  else
  {
    position->m_en_passant_square = INVALID_SQUARE;
  }

  position->m_moves = std::stoi(fen_parts.at(4));
  return position;
}

std::string position_to_fen(std::shared_ptr<Position> position)
{
  std::stringstream ss;
  int empty_square_count = 0;
  square_t square = 0x70;

  // pieces on the board
  while (square != 0x8)
  {
    if (is_invalid_square(square))
    {
      if (empty_square_count)
      {
        ss << empty_square_count;
        empty_square_count = 0;
      }
      ss << "/";
      square -= 0x18;
      continue;
    }

    if (position->m_mailbox[square] == VOID_PIECE)
    {
      empty_square_count++;
    }
    else
    {
      if (empty_square_count)
      {
        ss << empty_square_count;
        empty_square_count = 0;
      }
      ss << piece_to_char(position->m_mailbox[square]);
    }
    square++;
  }

  if (empty_square_count != 0)
  {
    ss << empty_square_count;
  }

  // whose turn
  ss << " " << (position->m_whites_turn ? "w " : "b ");

  bool some_castling = false;
  // castling rights
  if (position->m_white_kingside_castle)
  {
    ss << "K";
    some_castling = true;
  }
  if (position->m_white_queenside_castle)
  {
    ss << "Q";
    some_castling = true;
  }
  if (position->m_black_kingside_castle)
  {
    ss << "k";
    some_castling = true;
  }
  if (position->m_black_queenside_castle)
  {
    ss << "q";
    some_castling = true;
  }

  if (!some_castling)
  {
    ss << "-";
  }

  // en passant square
  ss << " ";
  ss << (is_valid_square(position->m_en_passant_square)
             ? index_to_an_square(position->m_en_passant_square)
             : "-");

  // halfmove clock (TODO)
  ss << " ";
  ss << "0";

  ss << " ";
  ss << position->m_moves;

  return ss.str();
}
