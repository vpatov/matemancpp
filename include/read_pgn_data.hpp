#ifndef __READ_PGN_DATA_H__
#define __READ_PGN_DATA_H__

#include "move_generation.hpp"
#include "position.hpp"
#include <boost/algorithm/string.hpp>
#include <codecvt>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>

struct Game;

struct metadata_entry
{
  std::string key;
  std::string value;
};

struct move_edge
{
  uint64_t dest_hash;
  uint32_t times_played;
  // move key is bit-wise concatenation of
  // 0x00 + start_square + end_square + promotion_piece
  uint32_t move_key;
};

struct OpeningTablebase
{
  Game *current_game;
  std::unordered_map<uint64_t, std::vector<move_edge>> opening_tablebase;
};

struct Game
{
  std::vector<metadata_entry> metadata;
  Position position;
  bool eloOverThreshold;
  bool finishedReading;
  std::string result;
  std::vector<uint32_t> move_list;

  bool read_metadata_line(std::string &line)
  {
    const std::string metadata_line_regex =
        "^\\s*\\[(\\w+)\\s\"(.*?)\"\\]\\s*$";

    std::smatch matches;
    if (std::regex_match(line, matches, std::regex(metadata_line_regex)))
    {
      metadata_entry entry;
      entry.key = std::string(matches[1]);
      entry.value = std::string(matches[2]);
      metadata.push_back(entry);

      return true;
    }
    return false;
  }

  void process_player_move(std::string player_move, bool white)
  {
    uint32_t move_key;

    const std::string non_castling_move_regex =
        "([RNBKQ])?([a-h])?([1-8])?(x)?([a-h])"
        "([1-8])(=[RNBKQ])?([\\+\\#])?";
    const std::string castling_move_regex = "((O-O-O)|(O-O))([\\+\\#])?";
    boost::algorithm::trim(player_move);

    if (player_move.size() == 0)
    {
      return;
    }

    std::smatch matches;
    if (std::regex_match(player_move, matches,
                         std::regex(non_castling_move_regex)))
    {

      move_key = non_castling_move(matches, white);
    }
    else if (std::regex_match(player_move, matches,
                              std::regex(castling_move_regex)))
    {

      std::string kingside_castle = matches[1];
      std::string queenside_castle = matches[2];
      std::string check_or_mate = matches[3];
    }
    else
    {
      std::cout << "no match: " << player_move << std::endl;
    }

    // push the parsed move key to the move list
    std::cout << "Move: " << position.plies << std::endl;
    std::cout << "Pushing: " << std::hex << move_key << std::endl
              << std::dec;
    move_list.push_back(move_key);

    // TODO remove this!!!
    position.plies++;
  }

  char getc(int i, std::smatch &matches)
  {
    if (matches[i].length())
    {
      return matches[i].str().at(0);
    }
    return 0;
  }

  // This function calculates and returns the move key, which is a concatenation
  // of the source square, destination square, and promotion piece (complete
  // information necessary to understand a move). Non-trivial because this
  // involves a minimal amount of move logic in order to calculate correctly.
  // For instance assume you have an empty board except for two white rooks on
  // e4 and d4. Let's say the move played was Ra4. It is easy for us to see that
  // only the rook on the d file could have gone to a4, because the e file rook
  // is blocked by the d file rook. However, to program this correctly, I would
  // need to scan the rank, file, or diagonal (depending on the piece) to
  // determine this. To make matters worse, if two pieces of the same type could
  // potentially move to the same square, but moving one would be illegal due to
  // discovered check, then the move notation does not include the source
  // file/rank.
  // TODO: refactor into several smaller functions
  // TODO: create function that asserts that a given chess position is legal
  // (includes turn to move)
  uint32_t non_castling_move(std::smatch &matches, bool white)
  {
    std::cout << "PGN move: " << matches[0] << std::endl;

    uint8_t src_square = 0x7f;
    uint8_t dest_square = 0x7f;
    uint8_t promotion_piece = 0;
    Color color = white ? Color::WHITE : Color::BLACK;

    char piece_char = getc(1, matches);
    char src_file = getc(2, matches);
    char src_rank = getc(3, matches);
    char capture = getc(4, matches);
    char dest_file = getc(5, matches);
    char dest_rank = getc(6, matches);
    std::string promotion = matches[7];
    char check_or_mate = getc(8, matches);

    // Get the promotion piece
    if (promotion.size())
    {
      std::cout << "PROMOTION: (" << promotion << ")" << std::endl;
      promotion_piece = char_to_piece(promotion.at(1));
      // piece should always be uppercase
      assert(promotion_piece < PIECE_MASK);
    }

    // destination file and rank should be present in every non-castling move
    assert(dest_file && dest_rank);
    dest_square = an_square_to_index(dest_file, dest_rank);

    bool is_pawn_move = !piece_char;
    if (is_pawn_move)
    {
      // there should never be a src_rank when it's a pawn move
      assert(!src_rank);
      uint8_t target = white ? W_PAWN : B_PAWN;

      if (capture)
      {
        // if we are capturing, the src_file should be present
        assert(src_file);
        src_rank = white ? dest_rank - 1 : dest_rank + 1;
        src_square = an_square_to_index(src_file, src_rank);
        assert(position.mailbox[src_square] == target);
      }
      else
      {
        Direction direction = white ? Direction::DOWN : Direction::UP;
        uint8_t candidate_square = STEP_DIRECTION(direction, dest_square);
        if (position.mailbox[candidate_square] == target)
        {
          src_square = candidate_square;
        }
        else
        {
          // If the pawn didn't come from the square we just checked, that
          // square must be empty
          assert(position.mailbox[candidate_square] == 0);
          candidate_square = STEP_DIRECTION(direction, candidate_square);
          assert(position.mailbox[candidate_square] == target);
          src_square = candidate_square;
        }
      }
    }

    // There is only one king per side so this should be simple
    // There should never be ambiguity.
    else if (piece_char == KING_CHAR)
    {
      src_square = find_king(&position, white);
      assert(VALID_SQUARE(src_square));

      // assert that the square that we found the king at, is one square away from the square he supposedly moved to.
      bool found_orig = false;
      for (auto it = directions_vector.begin(); it != directions_vector.end(); it++)
      {
        uint8_t check = dest_square + direction_offset(*it);
        if (check == src_square)
        {
          found_orig = true;
          break;
        }
      }
      assert(found_orig);
    }

    // Very rare for both of src_file and src_rank to be present. Only for some
    // moves like Qc1c2 or Nf3d4 where there are multiple pieces of the same type
    // that could go to the same square, and both src file and src rank are necessary
    // for the move not to be ambigous. An example of this is move 54 in the custom
    // lichess game in the test pgn data.
    if (src_file && src_rank)
    {
      src_square = an_square_to_index(src_file, src_rank);
    }

    // PGN annotation only provides src rank and file if it is necessary to disambiguate.
    else if (piece_char == KNIGHT_CHAR)
    {
      if (src_file && src_rank)
      {
        src_square = an_square_to_index(src_file, src_rank);
      }
      // find all knights a knight move away from the dest square
      std::vector<uint8_t> knights;
      for (auto it = knight_move_offsets.begin(); it != knight_move_offsets.end(); it++)
      {
        uint8_t square = *it + dest_square;
        if (INVALID_SQUARE(square))
        {
          continue;
        }
        if (position.mailbox[square] == (white ? W_KNIGHT : B_KNIGHT))
        {
          knights.push_back(square);
        }
      }

      // We should always find at least one knight.
      assert(knights.size() > 0);

      // if there was only one knight, we're done
      if (knights.size() == 1)
      {
        src_square = knights.at(0);
      }

      // TODO:
      // if there is a src_rank or src_file, this should help disambiguate.
      // if one of the knights is blocking check, it cannot move.
      // use new function legal_position to assist with this.
      // LASTLEFTOFF
      else
      {
        if (matches[0].compare("Nge7") == 0)
        {
          std::cout << "breakpoint" << std::endl;
        }

        std::cout << "Found more than one knight:" << std::endl;
        for (auto it = knights.begin(); it != knights.end(); it++)
        {
          if ((src_file && index_to_an_file(*it) == src_file) || (src_rank && src_rank == index_to_an_rank(*it)))
          {
            // ensure the position is legal
            adjust_position(&position, *it, dest_square, 0);
            if (legal_position(&position, white))
            {
              src_square = *it;
              adjust_position(&position, dest_square, src_square, 0);
              break;
            }
            else
            {
              adjust_position(&position, dest_square, *it, 0);
            }
          }
        }
      }
    }

    else if (piece_char == BISHOP_CHAR)
    {
    }

    // src_square and dest_square should be sufficiently populated at this point
    // move key is bit-wise concatenation of
    // 0x00 + start_square + end_square + promotion_piece
    uint32_t move_key =
        (src_square << 16) + (dest_square << 8) + promotion_piece;

    assert(!INVALID_SQUARE(dest_square));
    if (INVALID_SQUARE(src_square))
    {
      std::cout << "Impl incomplete for move: " << matches[0] << std::endl;
    }
    else
    {
      std::cout << "Generated move: " << index_to_an_square(src_square)
                << " -> " << index_to_an_square(dest_square);
      if (promotion_piece)
      {
        std::cout << " Promotion: " << piece_to_char(promotion_piece);
      }
      std::cout << std::endl
                << std::endl;

      adjust_position(&this->position, src_square, dest_square,
                      promotion_piece);

      // print_position(&this->position);
      print_position_with_borders(&this->position);
        }

    return move_key;
  }

  void process_result(std::string resultstr)
  {
    result = std::move(resultstr);
    finishedReading = true;
  }

  bool read_game_move_line(std::string &line)
  {

    const std::string result_regex_str =
        R"((((?:1\/2|1|0)\s*\-\s*(?:1\/2|1|0)\s*$)|\*)?)";

    const std::regex game_line_regex(
        R"(\d+\.\s*([\w\-\+\#\=]+)\s([\w\-\+\#\=]+)?\s*)" + result_regex_str);

    bool is_game_line = false;
    std::smatch matches;
    while (std::regex_search(line, matches, game_line_regex))
    {
      is_game_line = true;
      process_player_move(matches[1], true);
      process_player_move(matches[2], false);
      if (matches[3].length() > 1)
      {
        process_result(matches[3]);
      }
      line = matches.suffix().str();
    }

    return is_game_line;
  }
};

void print_matches(std::smatch &matches);

#define ELO_THRESHOLD 2200

void read_pgn_file(std::string file_path);
void read_all_pgn_files();

#endif