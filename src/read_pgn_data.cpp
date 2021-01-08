#include "read_pgn_data.hpp"
#include <fstream>

void read_pgn_file(std::string file_path) {

  std::ifstream infile(file_path);
  std::vector<Game> games;
  games.emplace_back();
  bool reading_game = false;

  for (std::string line; getline(infile, line);) {
    if (line.length() < 2) {
      continue;
    }

    if (!reading_game && !games.back().read_metadata_line(line)) {
      reading_game = true;
    }

    if (reading_game) {
      bool is_game_line = games.back().read_game_move_line(line);
      assert(is_game_line);
      if (games.back().finishedReading) {
        std::cout << " Place back game: " << games.back().result << std::endl;
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
