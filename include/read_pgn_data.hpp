#ifndef __READ_PGN_DATA_H__
#define __READ_PGN_DATA_H__

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

struct metadata_entry {
  std::string key;
  std::string value;
};

struct move_edge {
  uint64_t dest_hash;
  uint32_t times_played;
  // move key is bit-wise concatenation of
  // 0x00 + start_square + end_square + promotion_piece
  uint32_t move_key;
};

struct OpeningTablebase {
  Game *current_game;
  std::unordered_map<uint64_t, std::vector<move_edge>> opening_tablebase;
};

struct Game {
  std::vector<metadata_entry> metadata;
  Position position;
  bool eloOverThreshold;
  bool finishedReading;
  std::string result;
  std::vector<uint32_t> move_list;

  bool read_metadata_line(std::string &line) {
    const std::string metadata_line_regex =
        "^\\s*\\[(\\w+)\\s\"(.*?)\"\\]\\s*$";

    std::smatch matches;
    if (std::regex_match(line, matches, std::regex(metadata_line_regex))) {
      metadata_entry entry;
      entry.key = std::string(matches[1]);
      entry.value = std::string(matches[2]);
      metadata.push_back(entry);

      return true;
    }
    return false;
  }

  void process_player_move(std::string player_move) {
    uint32_t move_key;

    const std::string non_castling_move_regex =
        "([RNBKQ])?([a-h])?([1-8])?(x)?([a-h])"
        "([1-8])(=[RNBKQ])?([\\+\\#])?";
    const std::string castling_move_regex = "((O-O-O)|(O-O))([\\+\\#])?";
    boost::algorithm::trim(player_move);

    if (player_move.size() == 0) {
      return;
    }

    std::smatch matches;
    if (std::regex_match(player_move, matches,
                         std::regex(non_castling_move_regex))) {

      move_key = non_castling_move(matches);

    } else if (std::regex_match(player_move, matches,
                                std::regex(castling_move_regex))) {

      std::string kingside_castle = matches[1];
      std::string queenside_castle = matches[2];
      std::string check_or_mate = matches[3];
    } else {
      std::cout << "no match: " << player_move << std::endl;
    }

    // push the parsed move key to the move list
    std::cout << "Pushing: " << std::hex << move_key << std::endl << std::dec;
    move_list.push_back(move_key);
  }

  char getc(int i, std::smatch &matches) {
    if (matches[i].length()) {
      return matches[i].str().at(0);
    }
    return 0;
  }

  // This function calculates and returns the move key, which is a concatenation
  // of the source square, destination square, and promotion peice (complete
  // information necessary to understand a move). Non-trivial because this
  // involves a minimal amount of move logic in order to calculate correctly.
  // For instance assume you have an empty board except for two white rooks on
  // e4 and d4. Let's say the move played was Ra4. It is easy for us to see that
  // only the rook on the d file could have gone to a4, because the e file rook
  // is blocked by the d file rook. However, to program this correctly, I would
  // need to scan the rank, file, or diagonal (depending on the piece) to
  // determine this.
  uint32_t non_castling_move(std::smatch &matches) {
    uint8_t src_square;
    uint8_t dest_square;
    uint8_t promotion_piece;

    char piece_char = getc(1, matches);
    char src_file = getc(2, matches);
    char src_rank = getc(3, matches);
    char capture = getc(4, matches);
    char dest_file = getc(5, matches);
    char dest_rank = getc(6, matches);
    std::string promotion = matches[7];
    char check_or_mate = getc(8, matches);

    // Get the promotion piece
    if (promotion.size()) {
      promotion_piece = char_to_piece(promotion.at(1));
    }

    // destination file and rank should be present in every non-castling move
    assert(dest_file && dest_rank);
    bool is_pawn_move = !piece_char;
    if (is_pawn_move) {
      // there should never be a src_rank when it's a pawn move
      assert(!src_rank);

      // TODO: implement
    }

    // Very rare for both of src_file and src_rank to be present. Only for some
    // move like Qc1c2 (when theres multiple queens that could go to c2) where
    // both src file and src rank are necessary for the move not to be ambigous.
    // An example of this is move 54 in the custom lichess game in the test pgn
    // data.
    if (src_file && src_rank) {
      src_square = an_square_to_index(src_file, src_rank);
      dest_square = an_square_to_index(dest_file, dest_rank);
    } else if (src_file) {
      // TODO: implement
    } else {
      // TODO: implement
    }

    // src_square and dest_square should be sufficiently populated at this point
    // move key is bit-wise concatenation of
    // 0x00 + start_square + end_square + promotion_piece
    uint32_t move_key =
        (src_square << 16) + (dest_square << 8) + promotion_piece;

    return move_key;
  }

  void process_result(std::string resultstr) {
    result = std::move(resultstr);
    finishedReading = true;
  }

  bool read_game_move_line(std::string &line) {

    const std::string result_regex_str =
        R"((((?:1\/2|1|0)\s*\-\s*(?:1\/2|1|0)\s*$)|\*)?)";

    const std::regex game_line_regex(
        R"(\d+\.\s*([\w\-\+\#\=]+)\s([\w\-\+\#\=]+)?\s*)" + result_regex_str);

    bool is_game_line = false;
    std::smatch matches;
    while (std::regex_search(line, matches, game_line_regex)) {
      is_game_line = true;
      process_player_move(matches[1]);
      process_player_move(matches[2]);
      if (matches[3].length() > 1) {
        process_result(matches[3]);
      }
      line = matches.suffix().str();
    }

    return is_game_line;
  }
};

void print_matches(std::smatch &matches) {
  std::cout << matches[0] << std::endl;
  std::cout << "1: " << matches[1] << std::endl;
  std::cout << "2: " << matches[2] << std::endl;
  std::cout << "3: " << matches[3] << std::endl;
  std::cout << "4: " << matches[4] << std::endl;
  std::cout << "5: " << matches[5] << std::endl;
  std::cout << "6: " << matches[6] << std::endl;
  std::cout << "7: " << matches[7] << std::endl;
  std::cout << "8: " << matches[8] << std::endl;
}

#define ELO_THRESHOLD 2200

void read_pgn_file(std::string file_path);
void read_all_pgn_files();

#endif