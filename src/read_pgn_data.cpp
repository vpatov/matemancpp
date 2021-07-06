#include "read_pgn_data.hpp"
#include <fstream>

void read_pgn_file(std::string file_path) {

  std::ifstream infile(file_path);
  std::vector<Game> games;
  games.emplace_back();
  bool reading_game = false;
  bool elo_calc_done = false;
  int whiteElo;
  int blackElo;

  for (std::string line; getline(infile, line);) {
    if (line.length() < 2) {
      continue;
    }

    // read the metadata until there are no more metadata lines left
    if (!reading_game && !games.back().read_metadata_line(line)) {
      reading_game = true;
    }

    // before we start reading the game, find the elo of the players
    if (reading_game && !elo_calc_done) {

      // Iterate through metadata key-value pairs
      auto metadata = games.back().metadata;
      for (auto it = metadata.begin(); it != metadata.end(); it++) {
        if (it->value.length() < 2) {
          continue;
        }
        if ((it->key).compare("WhiteElo") == 0) {
          whiteElo = std::stoi(it->value);
          std::cout << "WhiteElo: " << whiteElo << std::endl;
        } else if ((it->key).compare("BlackElo") == 0) {
          blackElo = std::stoi(it->value);
          std::cout << "BlackElo: " << blackElo << std::endl;
        }
      }
      elo_calc_done = true;
    }

    if (reading_game) {

      bool is_game_line = games.back().read_game_move_line(line);
      assert(is_game_line);
      if (games.back().finishedReading) {
        std::cout << " Place back game: " << games.back().result << std::endl;

        // Remove the game if the players are beneath the elo threshold.
        if (whiteElo < ELO_THRESHOLD || blackElo < ELO_THRESHOLD) {
          std::cout << "Elos do not meet the threshold: " << whiteElo << ", "
                    << blackElo << std::endl;
          games.pop_back();
        }

        // push a new game to the back of the games vector
        games.emplace_back();
        reading_game = false;
      }
    }
  }
  std::cout << "Done" << std::endl;
}

void read_all_pgn_files() {
  read_pgn_file("/home/vas/repos/matemancpp/database/pgn/Stein.pgn");
  // read_pgn_file("/home/vas/repos/matemancpp/database/pgn/zzztest.pgn");

  if (true) {
    return;
  }
  std::string path = "/home/vas/repos/matemancpp/database/pgn";
  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    std::cout << entry.path() << std::endl;
    read_pgn_file(entry.path());
  }
}
