#ifndef __PARSE_PGN_H__
#define __PARSE_PGN_H__

#include "parse_pgn.hpp"
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

/*

uint8_t start_square
uint8_t end_square
uint8_t promotion_piece (0 if no promotion)
using move_key = uint32_t (concatenation of above);

struct move {
  uint64_t dest_hash
  uint32_t times_played
}


struct node {

  uint64_t hash (maybe not stored in node)
  position (entire copy?, probably not)

  // hash and position are technically not necessary to store, if
  // we arrive at a position always walking from starting position

  hashmap<move_key, uint64_t> move_map;
  // might be better to use a vector and not a move_map
}
*/

// TODO
/*
  Probably want to create data structure that parser populates


  For each game:
    position = new Position in starting position:
    current_node = opening_tablebase[starting_position]
    If no result or elo below threshold: skip
    For every move in move list:
      key = (from_square, dest_square, promotion)
      move = current_node.move_map.get(key, NULL);

      position = next_position(key)

      if (move != NULL){
        move.times_played ++;
        currrent_node = opening_tablebase[move.dest_hash];
      }
      else {
        hash = calculate_zobrist_hash(position)
        move = {
          dest_hash : hash,
          times_played : 1
        };
        current_node.move_map.set(key, move);
      }
*/

using result_data = std::vector<metadata_entry>;

template <typename Rule> struct action {};

template <> struct action<metadata_key> {
  template <typename ActionInput>
  static void apply(const ActionInput &in, result_data &data) {
    struct metadata_entry entry;
    entry.key = in.string();
    data.push_back(entry);
  }
};

template <> struct action<metadata_val> {
  template <typename ActionInput>
  static void apply(const ActionInput &in, result_data &data) {
    std::string res = in.string();
    data.back().value = res.substr(1, res.size() - 2);
  }
};

// template <> struct action<metadata_key> {
//   template <typename ActionInput>
//   static void apply(const ActionInput &in, result_data &data) {
//     struct metadata_entry entry;
//     entry.key = in.string();
//     data.push_back(entry);
//   }
// };

// template <> struct action<metadata_val> {
//   template <typename ActionInput>
//   static void apply(const ActionInput &in, result_data &data) {
//     // std::string res = in.string();
//     data.back().value = res.substr(1, res.size() - 2);
//   }
// };

template <> struct action<player_move> {
  template <typename ActionInput>
  static void apply(const ActionInput &in, result_data &data) {
    // std::cout << "player_move: '" << in.string() << "'" << std::endl;
  }
};

template <> struct action<game_move> {
  template <typename ActionInput>
  static void apply(const ActionInput &in, result_data &data) {
    // std::cout << "game_move: '" << in.string() << "'" << std::endl;
  }
};

template <> struct action<game_result> {
  template <typename ActionInput>
  static void apply(const ActionInput &in, result_data &data) {
    // std::cout << "game_result: '" << in.string() << "'" << std::endl;
  }
};

template <> struct action<game_moves> {
  template <typename ActionInput>
  static void apply(const ActionInput &in, result_data &data) {
    // std::cout << "game_moves: '" << in.string() << "'" << std::endl;
  }
};

template <> struct action<game> {
  template <typename ActionInput>
  static void apply(const ActionInput &in, result_data &data) {
    // std::string temp = in.string();

    // std::cout << "game: \'" << temp.substr(0, 20) << "... ..."
    //           << temp.substr(temp.size() - 20) << "'" << std::endl;
  }
};

template <> struct action<pgn_group> {
  template <typename ActionInput>
  static void apply(const ActionInput &in, result_data &data) {
    // std::cout << "pgn_group: '" << in.string() << "'" << std::endl;
  }
};

} // namespace pgn_parser

void parse_pgn_game();
void parse_pgn_moves();
void parse_pgn_metadata();
void parse_pgn_file(std::string file_path);
void parse_all_pgn_files();
#endif