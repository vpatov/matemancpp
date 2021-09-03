#include "read_pgn_data.hpp"
#include "pgn_game.hpp"
#include "threadpool.hpp"
#include <chrono>
#include <utility>

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

std::unordered_multimap<
    std::thread::id,
    std::shared_ptr<std::vector<std::shared_ptr<PgnGame>>>>
    pgn_game_vector_map;

void save_game_vector(std::thread::id thread_id, std::shared_ptr<std::vector<std::shared_ptr<PgnGame>>> games)
{
  static std::mutex pgn_game_vector_map_mutex;
  std::unique_lock<std::mutex> lock(pgn_game_vector_map_mutex);

  pgn_game_vector_map.insert(std::pair(thread_id, games));
}

void print_game_summaries(
    std::__1::chrono::steady_clock::time_point clock_start,
    std::__1::chrono::steady_clock::time_point clock_end,
    std::thread::id thread_id,
    int games_list_size,
    std::string file_path)
{
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start);
  std::string duration_str = std::to_string((double)duration.count() / 1000);
  duration_str = duration_str.substr(0, duration_str.find(".") + 3);

  std::cout
      << std::left << std::setw(16) << thread_id
      << std::left << std::setw(16) << duration_str + "s"
      << std::left << std::setw(16) << games_list_size
      << ColorCode::green
      << file_path.substr(file_path.find_last_of('/') + 1)
      << ColorCode::end
      << std::endl;
}

void read_pgn_file(std::string file_path)
{
  auto clock_start = std::chrono::high_resolution_clock::now();
  std::ifstream infile(file_path);
  std::shared_ptr<std::vector<std::shared_ptr<PgnGame>>> games =
      std::make_shared<std::vector<std::shared_ptr<PgnGame>>>();
  OpeningTablebase openingTablebase;

  games->emplace_back(std::make_shared<PgnGame>());
  populate_starting_position(&(games->back()->m_position));
  games->back()->m_opening_tablebase = &openingTablebase;
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
    if (!reading_game_moves && !games->back()->read_metadata_line(line))
    {
      reading_game_moves = true;
    }

    if (reading_game_moves)
    {

      bool is_game_line = games->back()->read_game_move_line(line);
      if (games->back()->m_finishedReading)
      {

        games->back()->populateMetadata();

        // push a new game to the back of the games vector
        games->emplace_back(std::make_shared<PgnGame>());

        // std::cerr << "\r"
        //           << std::left << std::setw(7)
        //           << games.size() - 1 << "\u001b[31m " << file_path << "\u001b[0m";

        populate_starting_position(&(games->back()->m_position));
        games->back()->m_opening_tablebase = &openingTablebase;
        reading_game_moves = false;
      }
    }
  }
  // Remove the very last game, which is empty
  games->pop_back();

  // Save the game vector in a global data structure so the main thread can access it
  save_game_vector(std::this_thread::get_id(), games);

  std::string tablebase_filepath = "/Users/vas/repos/matemancpp/tablebase/test.tb";
  openingTablebase.serialize_tablebase(tablebase_filepath);

  OpeningTablebase test_tablebase = OpeningTablebase::deserialize_tablebase(tablebase_filepath);

  std::cout << ColorCode::red << "Walking original tablebase: " << std::endl;
  openingTablebase.walk_down_most_popular_path();

  std::cout << ColorCode::end << ColorCode::blue << "Walking deserialized tablebase: " << std::endl;
  test_tablebase.walk_down_most_popular_path();

  std::cout << ColorCode::end << std::endl;

  // print statistics about pgn processing
  // auto clock_end = std::chrono::high_resolution_clock::now();
  // print_game_summaries(clock_start, clock_end, std::this_thread::get_id(), games->size(), file_path);
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

void print_game_summaries()
{
  for (auto member : pgn_game_vector_map)
  {
    auto first = member.first;
    auto second = member.second;

    std::cout << ColorCode::red << "Thread: " << first << ColorCode::end << std::endl;
    PgnGame::printGameSummaryHeader();
    for (auto vec_member : *second)
    {
      vec_member->printGameSummary();
    }
  }
}

void print_pgn_processing_header()
{
  std::cout
      << "\u001b[33m"
      << std::left << std::setw(16) << "Thread ID"
      << std::left << std::setw(16) << "Duration"
      << std::left << std::setw(16) << "# of Games"
      << std::left << std::setw(16) << "File Name"
      << "\u001b[0m"
      << std::endl;
}

void start_pgn_processing_tasks()
{
  ThreadPool thread_pool = ThreadPool();

  // print_pgn_processing_header();

  // for (const auto &entry : std::filesystem::directory_iterator(pgn_database_path))
  for (const auto &entry : std::filesystem::directory_iterator("/Users/vas/repos/matemancpp/database/subtest2"))
  {
    Task task = Task(&read_pgn_file, entry.path());
    thread_pool.add_task(task);
  }
  thread_pool.join_pool();

  // print_game_summaries();
}

void read_all_pgn_files()
{

  // read_pgn_file("/Users/vas/repos/matemancpp/database/pgn/Carlsen.pgn");
  // read_pgn_file("/Users/vas/repos/matemancpp/database/pgn/zzztest.pgn");
  // openingTablebase.walk_down_most_popular_path();
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
    // std::cerr << "Completed: " << entry.path() << std::endl;
    // update_completed_files_set(entry.path(), &completed_files);
  }
}
