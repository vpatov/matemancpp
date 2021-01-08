#ifndef __PARSE_PGN_H__
#define __PARSE_PGN_H__

#include "parse_pgn.hpp"
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
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/analyze.hpp>
#include <unordered_map>

#define ELO_THRESHOLD 2200

class OpeningTablebase;
void parse_pgn_game();
void parse_pgn_moves();
void parse_pgn_metadata();
void parse_pgn_file(std::string file_path);
void parse_all_pgn_files();
void process_player_move(std::string game_move,
                         OpeningTablebase &openingTablebase);

struct move_edge {
  uint64_t dest_hash;
  uint32_t times_played;
  // move key is concatenation of
  // 0x00 + start_square + end_square + promotion_piece
  uint32_t move_key;
};

// using position_node = ;
// move_map;

namespace pegtl = TAO_PEGTL_NAMESPACE;

namespace pgn_parser {

using namespace tao::pegtl;

struct metadata_val : seq<one<'"'>, until<at<one<'"'>>, any>, any> {};
struct metadata_key : plus<alpha> {};
struct metadata_line
    : seq<one<'['>, metadata_key, space, metadata_val, one<']'>, pegtl::eol> {};

struct pgn_group : pegtl::until<rep_min<1, eol>, metadata_line> {};

struct sep : ascii::space {};
struct file : utf8::range<'a', 'h'> {};
struct rank : utf8::range<'1', '8'> {};
struct piece : one<'K', 'Q', 'R', 'N', 'B'> {};
struct capture : one<'x'> {};

struct dest_square : seq<file, rank> {};

struct mate : one<'#'> {};
struct check : one<'+'> {};
struct check_or_mate : sor<check, mate> {};

struct kingside_castle : seq<one<'O'>, one<'-'>, one<'O'>> {};
struct queenside_castle : seq<kingside_castle, one<'-'>, one<'O'>> {};

struct promotion_move
    : seq<rep_max<1, seq<file, capture>>, dest_square, one<'='>, piece> {};
struct castle_move : sor<queenside_castle, kingside_castle> {};
struct normal_move : star<sor<alpha, digit>> {
}; // started normal_move by trying to actually represent from_square,
   // dest_square, etc, but it ended up getting too complicated

struct player_move : seq<sor<promotion_move, castle_move, normal_move>,
                         rep_max<1, check_or_mate>, plus<sep>> {};
struct game_move : seq<plus<digit>, one<'.'>, rep_min_max<1, 2, player_move>> {
};

struct one_half : seq<one<'1'>, one<'/'>, one<'2'>> {};
struct draw : seq<one_half, one<'-'>, one_half> {};
struct no_result : one<'*'> {};
struct white_win : seq<one<'1'>, one<'-'>, one<'0'>> {};
struct black_win : seq<one<'0'>, one<'-'>, one<'1'>> {};
struct game_result : seq<sor<white_win, black_win, draw, no_result>, eol> {};

struct game_moves : until<game_result, game_move> {};
struct game : must<pgn_group, game_moves, star<eol>> {};
struct pgn_file : until<eof, game> {};

struct metadata_entry {
  std::string key;
  std::string value;
};

struct Game {
  std::vector<metadata_entry> metadata;
  Position position;
  bool eloOverThreshold;
};

class OpeningTablebase {
  Game current_game;
  std::unordered_map<uint64_t, std::vector<move_edge>> opening_tablebase;
};

template <typename Rule> struct action {};

template <> struct action<metadata_key> {
  template <typename ActionInput>
  static void apply(const ActionInput &in, OpeningTablebase &tablebase) {
    struct metadata_entry entry;
    entry.key = in.string();
    tablebase.current_game.metadata.push_back(entry);
  }
};

template <> struct action<metadata_val> {
  template <typename ActionInput>
  static void apply(const ActionInput &in, OpeningTablebase &tablebase) {
    std::string res = in.string();
    tablebase.current_game.metadata.back().value =
        res.substr(1, res.size() - 2);
  }
};

template <> struct action<pgn_group> {
  template <typename ActionInput>
  static void apply(const ActionInput &in, OpeningTablebase &tablebase) {
    std::cout << "pgn_group" << std::endl;
    auto metadata = tablebase.current_game.metadata;
    int blackElo = 0;
    int whiteElo = 0;
    for (auto it = metadata.begin(); it != metadata.end(); it++) {
      if (it->key.compare("WhiteElo")) {
        whiteElo = std::stoi(it->value);
        std::cout << "White elo: " << whiteElo << std::endl;
      } else if (it->key.compare("BlackElo")) {
        blackElo = std::stoi(it->value);
      }
    }
    if (blackElo > ELO_THRESHOLD && whiteElo > ELO_THRESHOLD) {
      tablebase.current_game.eloOverThreshold = true;
    }
  }
};

template <> struct action<player_move> {
  template <typename ActionInput>
  static void apply(const ActionInput &in, OpeningTablebase &tablebase) {
    // std::cout << "player_move: '" << in.string() << "'" << std::endl;
    if (tablebase.current_game.eloOverThreshold) {
      process_player_move(in.string(), tablebase);
    }
  }
};

template <> struct action<game> {
  template <typename ActionInput>
  static void apply(const ActionInput &in, OpeningTablebase &tablebase) {
    std::cout << "game: '" << in.string() << "'" << std::endl;
  }
};

} // namespace pgn_parser

#endif