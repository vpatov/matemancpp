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
  // move key is concatenation of
  // 0x00 + start_square + end_square + promotion_piece
  uint32_t move_key;
};

// 1.e4 Nf6 2.e5 Nd5 3.d4 d6 4.Nf3 g6 5.Be2 Bg7 6.c4 Nb6 7.exd6 cxd6 8.h3 O-O
// 9.O-O Nc6 10.Nc3 Bf5 11.b3 e5 12.Be3 Re8 13.Rc1 e4 14.Ne1 d5 15.c5 Nc8 16.Nc2
// Be6 17.f3 exf3 18.Rxf3 N8e7 19.g4 f5 20.g5 f4 21.Bxf4 Nf5 22.Nb5 Rf8 23.Qd2
// Rf7 24.Rd3 a6 25.Nd6 Nxd6 26.Bxd6 Bf5 27.Rf3 Bxc2 28.Rxf7 Kxf7 29.Rxc2
// Nxd4 30.Qf4+ Kg8 31.Rd2 Qe8 32.Rxd4 Qxe2 33.Rxd5 Qe6 34.Rd3 Qe1+ 35.Qf1
// Qe4 36.Qf3 Qe1+ 37.Qf1 Qe4 38.Qf3 Qe1+ 39.Qf1 Qe4 40.Qf3 Qe1+  1/2-1/2
struct Game {
  std::vector<metadata_entry> metadata;
  Position position;
  bool eloOverThreshold;
  bool finishedReading;
  std::string result;

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
      // std::cout << "matches non_castling_move" << std::endl;

      non_castling_move(matches);

    } else if (std::regex_match(player_move, matches,
                                std::regex(castling_move_regex))) {

      std::string kingside_castle = matches[1];
      std::string queenside_castle = matches[2];
      std::string check_or_mate = matches[3];
    } else {
      std::cout << "no match: " << player_move << std::endl;
    }
  }

  char getc(int i, std::smatch &matches) {
    if (matches[i].length()) {
      return matches[i].str().at(0);
    }
    return 0;
  }

  // e4
  // cxd4
  void non_castling_move(std::smatch &matches) {
    // std::cout << "full match: " << matches[0] << std::endl;
    // std::cout << "1: " << matches[1] << std::endl;
    // std::cout << "2: " << matches[2] << std::endl;
    // std::cout << "3: " << matches[3] << std::endl;
    // std::cout << "4: " << matches[4] << std::endl;
    // std::cout << "5: " << matches[5] << std::endl;
    // std::cout << "6: " << matches[6] << std::endl;
    // std::cout << "7: " << matches[7] << std::endl;
    // std::cout << "8: " << matches[8] << std::endl;

    char piecec = getc(1, matches);
    char src_file = getc(2, matches);
    char src_rank = getc(3, matches);
    char capture = getc(4, matches);
    char dest_file = getc(5, matches);
    char dest_rank = getc(6, matches);
    std::string promotion = matches[7];
    char check_or_mate = getc(8, matches);

    if (promotion.size()) {
    }

    // These should be present in every non-castling move
    assert(dest_file && dest_rank);
    bool is_pawn_move = !piecec;
    if (is_pawn_move) {
      // there should never be a src_rank when it's a pawn move
      assert(!src_rank);
    }
    uint8_t src_square;
    uint8_t dest_square;
    uint8_t piece;

    // Very rare for both of these to be present. Only for some move like
    // Qc1c2 (when theres multiple queens that could go to c2)
    // where both src file and src rank are necessary for the move not to be
    // ambigous
    if (src_file && src_rank) {
      src_square = an_square_to_index(src_file, src_rank);
      dest_square = an_square_to_index(dest_file, dest_rank);
    } else if (src_file) {
    }
  }

  /*
  regex101 working regex:
  \d+\.([\w\-\+\#]+)\s([\w\-\+\#]+)?\s*((?:1\/2|1|0)\s*\-\s*(?:1\/2|1|0)\s*$)?

  */

  void process_result(std::string resultstr) {
    result = std::move(resultstr);
    finishedReading = true;
  }

  bool read_game_move_line(std::string &line) {

    const std::string result_regex_str =
        R"((((?:1\/2|1|0)\s*\-\s*(?:1\/2|1|0)\s*$)|\*)?)";

    const std::regex game_line_regex(
        R"(\d+\.([\w\-\+\#\=]+)\s([\w\-\+\#\=]+)?\s*)" + result_regex_str);

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

class OpeningTablebase {
  Game current_game;
  std::unordered_map<uint64_t, std::vector<move_edge>> opening_tablebase;
};

#define ELO_THRESHOLD 2200

void read_pgn_file(std::string file_path);
void read_all_pgn_files();

#endif