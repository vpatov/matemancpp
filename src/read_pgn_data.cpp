#include "read_pgn_data.hpp"

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

OpeningTablebase openingTablebase;

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
  std::vector<std::unique_ptr<Game>> games;

  games.emplace_back(std::make_unique<Game>());
  populate_starting_position(&(games.back()->position));
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
    if (!reading_game_moves && !games.back()->read_metadata_line(line))
    {
      reading_game_moves = true;
    }

    if (reading_game_moves)
    {

      bool is_game_line = games.back()->read_game_move_line(line);
      if (games.back()->finishedReading)
      {

        populateMetadata(games.back().get());

        // push a new game to the back of the games vector
        games.emplace_back(std::make_unique<Game>());

        std::cerr << "\r"
                  << std::left << std::setw(7)
                  << games.size() - 1 << "\u001b[31m " << file_path << "\u001b[0m";

        populate_starting_position(&(games.back()->position));
        reading_game_moves = false;
      }
    }
  }

  // std::cerr << std::endl;
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

  read_pgn_file("/Users/vas/repos/matemancpp/database/pgn/Carlsen.pgn");
  // read_pgn_file("/Users/vas/repos/matemancpp/database/pgn/zzztest.pgn");
  openingTablebase.try_all_paths();
  exit(0);

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
    // std::cerr << "Completed: " << entry.path() << std::endl;
    // update_completed_files_set(entry.path(), &completed_files);
  }
}

bool Game::read_metadata_line(std::string &line)
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

void Game::process_player_move(std::string player_move, bool whites_turn)
{
  uint32_t move_key;
  boost::algorithm::trim(player_move);

  if (player_move.size() == 0)
  {
    return;
  }

  position.m_whites_turn = whites_turn;

  z_hash_t zhash1 = zobrist_hash(&position);

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

      if (!whites_turn)
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
    move_key = position.castling_move(matches, whites_turn);
  }
  else
  {
    // expected regex to match a move.
    assert(false);
  }
  // push the parsed move key to the move list
  move_list.push_back(move_key);
  position.m_plies++;

  position.m_whites_turn = !whites_turn;
  z_hash_t zhash2 = zobrist_hash(&position);

  openingTablebase.update(zhash1, zhash2, move_key, std::string(player_move));
}

bool Game::read_game_move_line(std::string &line)
{

  bool is_game_line = false;
  std::smatch matches;
  while (std::regex_search(line, matches, game_line_regex))
  {
    is_game_line = true;
    // std::cout << "=================================" << std::endl;
    if (!metadata.size())
    {
      std::cerr << "No metadata!" << std::endl;
      assert(false);
    }
    // std::cout << (metadata.size() ? metadata.at(0).value : "No metadata") << std::endl;
    // std::cout << matches[0] << std::endl;

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

void Game::process_result(std::string resultstr)
{
  result = std::move(resultstr);
  finishedReading = true;
}
