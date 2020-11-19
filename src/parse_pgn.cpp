#include "parse_pgn.hpp"
#include <tao/pegtl/contrib/trace.hpp>

// std::string src_file = matches[1];
// std::string src_rank = matches[2];
// std::string capture = matches[3];
// std::string dest_file = matches[4];
// std::string dest_rank = matches[5];
// std::string promotion = matches[6];
// std::string check_or_mate = matches[7];

// void adjust_position(unique_ptr<Position> position,
//                      unique_ptr<opening_tablebase> tablebase, char src_file,
//                      char src_rank, char capture, char dest_file,
//                      char dest_rank, std::string promotion,
//                      char check_or_mate) {}

void non_castling_move(std::smatch &matches, OpeningTablebase &tablebase) {
  char src_file = matches[1].str().at(0);
  char src_rank = matches[2].str().at(0);
  char capture = matches[3].str().at(0);
  char dest_file = matches[4].str().at(0);
  char dest_rank = matches[5].str().at(0);
  std::string promotion = matches[6];
  char check_or_mate = matches[7].str().at(0);
}

// using position_node = std::vector<uint32_t, move_edge> move_map;
// using opening_tablebase = std::unordered_map<uint64_t, position_node>;

void process_player_move(std::string pgn_player_move,
                         pgn_parser::OpeningTablebase &openingTablebase) {

  const std::string player_move =
      ".*([a-h])?([1-8])?(x)?([a-h])([1-8])(\\=[RNBKQ])?([\\+\\#])?";
  const std::string castle_move = "(O-O)||(O-O-O)([\\+\\#])?";
  boost::algorithm::trim(pgn_player_move);

  std::smatch matches;
  if (std::regex_match(pgn_player_move, matches, std::regex(player_move))) {

  } else if (std::regex_match(castle_move, matches, std::regex(player_move))) {

    std::string kingside_castle = matches[1];
    std::string queenside_castle = matches[2];
    std::string check_or_mate = matches[3];
  }
}

namespace fs = std::filesystem;

void parse_all_pgn_files() {
  // parse_pgn_file("/home/vas/repos/matemancpp/database/pgn/Stein.pgn");

  // if (true) {
  //   return;
  // }
  std::string path = "/home/vas/repos/matemancpp/database/pgn";
  for (const auto &entry : fs::directory_iterator(path)) {
    std::cout << entry.path() << std::endl;
    parse_pgn_file(entry.path());
    // exit(0);
  }
}

void parse_pgn_file(std::string file_path) {

  // Check the grammar for some possible issues.

  if (pegtl::analyze<pgn_parser::pgn_file>() != 0) {
    std::cout << "pgn_file grammar failed analysis. " << std::endl;
    exit(1);
  }

  pgn_parser::OpeningTablebase openingTablebase;

  pegtl::file_input in(file_path);

  // TODO why doesnt this parse anything? is some template wrong?
  try {
    pegtl::complete_trace<pgn_parser::pgn_file>(in);
    // pegtl::parse<pgn_parser::pgn_file, pgn_parser::action>(in,
    //                                                        &openingTablebase);

  } catch (const pegtl::parse_error &e) {
    const auto p = e.positions().front();
    std::cerr << e.what() << std::endl
              << in.line_at(p) << '\n'
              << std::setw(p.column) << '^' << std::endl;
  }
}

void parse_pgn_metadata() {}
void parse_pgn_moves() {}

/*

TODO use regex instead of pegtl parser to simplify everything. pegtl is slowing
you down more than its helping, and its not the point of the project. It's also
not particularly fast

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
// 1) implement next_position(from_square, dest_square, promotion)
// 2) implement get_fromsq_destsq_promotion(playerMove)
// 3) think about threading and locks for this code below
// 4) Think about using regex instead of a parser
/*



  For each game:
    position = new Position in starting position:
    current_node* = opening_tablebase[starting_position]
    If no result or elo below threshold: skip
    For every move in move list:
      key = (from_square, dest_square, promotion)
      move* = current_node.move_map.get(key, NULL);

      position = next_position(key)

      if (move != NULL){
        move->times_played ++;
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
