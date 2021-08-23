#ifndef __READ_PGN_DATA_H__
#define __READ_PGN_DATA_H__

#include "move_generation.hpp"
#include "position.hpp"
#include "util.hpp"
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

const std::string result_regex_str =
    R"((((?:1\/2|1|0)\s*\-\s*(?:1\/2|1|0)\s*$)|\*)?)";
const std::regex game_line_regex(
    R"(\d+\.\s*([\w\-\+\#\=]+)\s([\w\-\+\#\=]+)?\s*)" + result_regex_str);
const std::string metadata_line_regex =
    "^\\s*\\[(\\w+)\\s\"(.*?)\"\\]\\s*$";
const std::string non_castling_move_regex =
    "([RNBKQ])?([a-h])?([1-8])?(x)?([a-h])"
    "([1-8])(=[RNBKQ])?([\\+\\#])?";
const std::string castling_move_regex = "((O-O-O)|(O-O))([\\+\\#])?";

struct Game
{
  std::vector<metadata_entry> metadata;
  Position position;
  int whiteElo;
  int blackElo;
  bool eloOverThreshold;
  bool finishedReading;
  std::string result;
  std::vector<uint32_t> move_list;

  bool read_metadata_line(std::string &line)
  {
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
    boost::algorithm::trim(player_move);

    if (player_move.size() == 0)
    {
      return;
    }

    position.m_whites_turn = white;

    std::smatch matches;
    if (std::regex_match(player_move, matches,
                         std::regex(non_castling_move_regex)))
    {
      char piece_char = getc(1, matches);
      char src_file = getc(2, matches);
      char src_rank = getc(3, matches);
      char capture = getc(4, matches);
      char dest_file = getc(5, matches);
      char dest_rank = getc(6, matches);
      std::string promotion = matches[7];
      char promotion_piece = 0;
      char check_or_mate = getc(8, matches);

      // Get the promotion piece
      if (promotion.size())
      {
        promotion_piece = char_to_piece(promotion.at(1));
        // piece should always be uppercase because pieces are uppercase in PGN.
        assert(promotion_piece < PIECE_MASK);

        if (!white)
        {
          promotion_piece |= BLACK_PIECE_MASK;
        }
      }

      move_key = position.non_castling_move(
          piece_char, src_file, src_rank,
          capture, dest_file, dest_rank,
          promotion_piece, check_or_mate);
    }
    else if (std::regex_match(player_move, matches,
                              std::regex(castling_move_regex)))
    {
      move_key = position.castling_move(matches, white);
    }
    else
    {
      // expected regex to match a move.
      assert(false);
    }

    // push the parsed move key to the move list
    move_list.push_back(move_key);

    position.m_plies++;
  }

  // Non-trivial because this
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
  // TODO: consider rewriting this but with using piece lists (would probably be much easier)
  // TODO: make it such that it has no side effects.

  void process_result(std::string resultstr)
  {
    result = std::move(resultstr);
    finishedReading = true;
  }

  bool read_game_move_line(std::string &line)
  {

    bool is_game_line = false;
    std::smatch matches;
    while (std::regex_search(line, matches, game_line_regex))
    {
      is_game_line = true;
      std::cout << "=================================" << std::endl;
      if (!metadata.size())
      {
        std::cerr << "No metadata!" << std::endl;
        assert(false);
      }
      std::cout << (metadata.size() ? metadata.at(0).value : "No metadata") << std::endl;
      std::cout << matches[0] << std::endl;

      process_player_move(matches[1], true);
      process_player_move(matches[2], false);
      if (matches[3].length() > 0)
      {
        process_result(matches[3]);
      }
      line = matches.suffix().str();
    }

    return is_game_line;
  }
};

#define ELO_THRESHOLD 2200

// std::vector<Game> read_pgn_file(std::string file_path);
void read_pgn_file(std::string file_path);
void read_all_pgn_files();

#endif