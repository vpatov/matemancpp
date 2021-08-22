#include "read_pgn_data.hpp"
#include <fstream>
#include <set>
#include <thread>

const std::string test_files[8] = {
    "/Users/vas/repos/matemancpp/database/pgn/Berliner.pgn",
    "/Users/vas/repos/matemancpp/database/pgn/DeLaBourdonnais.pgn",
    "/Users/vas/repos/matemancpp/database/pgn/McDonnell.pgn",
    "/Users/vas/repos/matemancpp/database/pgn/Horwitz.pgn",
    "/Users/vas/repos/matemancpp/database/pgn/Breyer.pgn",
    "/Users/vas/repos/matemancpp/database/pgn/Morphy.pgn",
    "/Users/vas/repos/matemancpp/database/pgn/MacKenzie.pgn",
    "/Users/vas/repos/matemancpp/database/pgn/Winawer.pgn",
};

const std::string completed_files_filepath = "/Users/vas/repos/matemancpp/completed_files.txt";
const std::string pgn_database_path = "/Users/vas/repos/matemancpp/database/pgn";

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

// Reads the elo, and other metadata.
void populateMetadata(Game *game)
{
  int elo;

  // Iterate through metadata key-value pairs
  auto metadata = game->metadata;
  for (auto it = metadata.begin(); it != metadata.end(); it++)
  {
    if (it->value.length() < 2)
    {
      continue;
    }
    if ((it->key).compare("WhiteElo") == 0)
    {
      elo = std::stoi(it->value);
      if (elo >= 0)
      {
        game->whiteElo = elo;
      }
    }
    else if ((it->key).compare("BlackElo") == 0)
    {
      elo = std::stoi(it->value);
      if (elo >= 0)
      {
        game->blackElo = elo;
      }
    }
  }
  game->eloOverThreshold =
      game->whiteElo >= ELO_THRESHOLD && game->blackElo >= ELO_THRESHOLD;
}

void read_pgn_file(std::string file_path)
{
  std::ifstream infile(file_path);
  std::vector<Game> games;

  games.emplace_back();
  populate_starting_position(&(games.back().position));
  bool reading_game_moves = false;
  int linecount = 0;

  if (!infile.is_open())
  {
    std::cerr << "Could not open " << file_path << std::endl;
    return;
  }
  for (std::string line; getline(infile, line);)
  {
    linecount++;
    // std::cout << file_path << ":" << linecount << std::endl;
    if (line.length() < 2)
    {
      continue;
    }

    // read the metadata until there are no more metadata lines left
    if (!reading_game_moves && !games.back().read_metadata_line(line))
    {
      reading_game_moves = true;
    }

    if (reading_game_moves)
    {

      bool is_game_line = games.back().read_game_move_line(line);
      // std::cout << "\u001b[32m"
      //           << "#games: " << games.size() << "\u001b[0m" << std::endl;

      // assert(is_game_line);
      if (games.back().finishedReading)
      {

        populateMetadata(&games.back());

        // push a new game to the back of the games vector
        games.emplace_back();
        std::cerr << "\r"
                  << std::left << std::setw(7)
                  << games.size() - 1 << "\u001b[31m " << file_path << "\u001b[0m";

        populate_starting_position(&(games.back().position));
        reading_game_moves = false;
      }
    }
  }
  std::cerr << std::endl;
}

/* 
  This function calculates and returns the move key, which is a concatenation 
  of the source square, destination square, and promotion piece (complete 
  information necessary to understand a move). For castling moves, even though
  two pieces move, the src_square and dest_square pertain only to the king.
*/
uint32_t generate_move_key(uint8_t src_square, uint8_t dest_square, uint8_t promotion_piece)
{
  // move key is bit-wise concatenation of
  // (empty/reserved) + start_square + end_square + promotion_piece
  // 8 bits             8 bits         8 bits       8 bits
  return (src_square << 16) + (dest_square << 8) + promotion_piece;
}

char getc(int i, std::smatch &matches)
{
  if (matches[i].length())
  {
    return matches[i].str().at(0);
  }
  return 0;
}

std::set<std::string> get_completed_files_set()
{
  std::ifstream infile(completed_files_filepath);
  std::set<std::string> completed_files;
  for (std::string line; getline(infile, line);)
  {
    completed_files.insert(line);
  }
  return completed_files;
}

void update_completed_files_set(std::string filename, std::set<std::string> *completed_files)
{
  std::ofstream ofs(completed_files_filepath, std::ofstream::trunc);
  completed_files->insert(filename);
  for (auto it = completed_files->begin(); it != completed_files->end(); it++)
  {
    ofs << *it << std::endl;
  }
  ofs.close();
}

void read_all_pgn_files()
{

  // read_pgn_file("/Users/vas/repos/matemancpp/database/pgn/zzztest.pgn");
  // exit(0);

  // std::set completed_files = get_completed_files_set();

  for (const auto &entry : std::filesystem::directory_iterator(pgn_database_path))
  {
    // if (completed_files.find(entry.path()) != completed_files.end())
    // {
    //   std::cout << "Skipping: " << entry.path() << std::endl;
    //   continue;
    // }

    std::cout << "\u001b[31m " << entry.path() << "\u001b[0m" << std::endl;
    read_pgn_file(entry.path());

    // Update set of completed files
    std::cerr << "Completed: " << entry.path() << std::endl;
    // update_completed_files_set(entry.path(), &completed_files);
  }
}
