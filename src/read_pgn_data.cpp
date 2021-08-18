#include "read_pgn_data.hpp"
#include <fstream>

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


// TODO
// 1) implement next_position(from_square, dest_square, promotion)
// 2) implement get_fromsq_destsq_promotion(playerMove)
// 3) think about threading and locks for this code below
// 4) Think about using regex instead of a parser



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

void read_pgn_file(std::string file_path)
{
  std::ifstream infile(file_path);
  std::vector<Game> games;
  games.emplace_back();
  populate_starting_position(&(games.back().position));
  bool reading_game = false;
  bool elo_calc_done = false;
  int whiteElo = 0;
  int blackElo = 0;

  for (std::string line; getline(infile, line);)
  {
    if (line.length() < 2)
    {
      continue;
    }

    // read the metadata until there are no more metadata lines left
    if (!reading_game && !games.back().read_metadata_line(line))
    {
      reading_game = true;
    }

    // before we start reading the game, find the elo of the players
    if (reading_game && !elo_calc_done)
    {

      // Iterate through metadata key-value pairs
      auto metadata = games.back().metadata;
      for (auto it = metadata.begin(); it != metadata.end(); it++)
      {
        if (it->value.length() < 2)
        {
          continue;
        }
        if ((it->key).compare("WhiteElo") == 0)
        {
          whiteElo = std::stoi(it->value);
          std::cout << "WhiteElo: " << whiteElo << std::endl;
        }
        else if ((it->key).compare("BlackElo") == 0)
        {
          blackElo = std::stoi(it->value);
          std::cout << "BlackElo: " << blackElo << std::endl;
        }
      }
      elo_calc_done = true;
    }

    if (reading_game)
    {

      bool is_game_line = games.back().read_game_move_line(line);
      std::cout << "\u001b[32m"
                << "#games: " << games.size() << "\u001b[0m" << std::endl;

      // assert(is_game_line);
      if (games.back().finishedReading)
      {
        std::cout << " Place back game: " << games.back().result << std::endl;

        games.back().eloOverThreshold =
            whiteElo >= ELO_THRESHOLD && blackElo >= ELO_THRESHOLD;

        // push a new game to the back of the games vector
        games.emplace_back();
        std::cout << "\u001b[31m " << file_path << "\u001b[0m" << std::endl;
        populate_starting_position(&(games.back().position));
        // exit(0);
        reading_game = false;
      }
    }
  }
  std::cout << "Done" << std::endl;
}

void print_matches(std::smatch &matches)
{
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

void read_all_pgn_files()
{
  // read_pgn_file("/Users/vas/repos/matemancpp/database/pgn/zzz_cur_test.pgn");
  // read_pgn_file("/Users/vas/repos/matemancpp/database/pgn/zzztest.pgn");
  // read_pgn_file("/Users/vas/repos/matemancpp/database/pgn/RuyLopezOpen.pgn");
  // read_pgn_file("/Users/vas/repos/matemancpp/database/pgn/Stein.pgn");

  // if (true)
  // {
  //   return;
  // }
  std::string path = "/Users/vas/repos/matemancpp/database/pgn";
  for (const auto &entry : std::filesystem::directory_iterator(path))
  {
    std::cout << "\u001b[31m " << entry.path() << "\u001b[0m" << std::endl;
    read_pgn_file(entry.path());
  }
}
