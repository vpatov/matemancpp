#include "read_pgn_data.hpp"
#include "pgn_game.hpp"
#include "threadpool.hpp"
#include "master_tablebase.hpp"
#include <chrono>
#include <utility>
#include <filesystem>

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

std::unordered_multimap<std::thread::id, std::shared_ptr<OpeningTablebase>> threads_tablebases;

MasterTablebase masterTablebase;

void print_pgn_processing_performance_summary(
    std::__1::chrono::steady_clock::time_point clock_start,
    std::__1::chrono::steady_clock::time_point clock_end,
    std::thread::id thread_id,
    int games_list_size,
    int tablebase_size,
    std::string file_path)
{
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start);
  std::string duration_str = std::to_string((double)duration.count() / 1000);
  duration_str = duration_str.substr(0, duration_str.find(".") + 3);

  std::cout
      << std::left << std::setw(16) << thread_id
      << std::left << std::setw(16) << duration_str + "s"
      << std::left << std::setw(16) << games_list_size
      << std::left << std::setw(16) << tablebase_size
      << ColorCode::green
      << file_path.substr(file_path.find_last_of('/') + 1)
      << ColorCode::end
      << std::endl;
}

void print_pgn_processing_header()
{
  std::cout
      << "\u001b[33m"
      << std::left << std::setw(16) << "Thread ID"
      << std::left << std::setw(16) << "Duration"
      << std::left << std::setw(16) << "# of Games"
      << std::left << std::setw(16) << "Tablebase Size"
      << std::left << std::setw(16) << "File Name"
      << "\u001b[0m"
      << std::endl;
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

void save_game_vector(std::thread::id thread_id, std::shared_ptr<std::vector<std::shared_ptr<PgnGame>>> games)
{
  static std::mutex pgn_game_vector_map_mutex;
  std::unique_lock<std::mutex> lock(pgn_game_vector_map_mutex);

  pgn_game_vector_map.insert(std::pair(thread_id, games));
}

void save_tablebase(std::thread::id thread_id, std::shared_ptr<OpeningTablebase> openingTablebasePtr)
{
  static std::mutex threads_tablebases_mutex;
  std::unique_lock<std::mutex> lock(threads_tablebases_mutex);

  threads_tablebases.insert(std::pair(thread_id, openingTablebasePtr));
}

std::string get_individual_tablebase_filepath(std::string pgn_file_path, std::string suffix)
{
  size_t path_end = pgn_file_path.rfind('/');
  size_t extension_start = pgn_file_path.rfind(".pgn");

  return latest_individual_tablebases_filepath + '/' +
         pgn_file_path.substr(
             path_end + 1,
             extension_start - path_end - 1) +
         suffix;
}

void process_pgn_file(std::string file_path)
{
  auto clock_start = std::chrono::high_resolution_clock::now();
  std::ifstream infile(file_path);
  std::shared_ptr<std::vector<std::shared_ptr<PgnGame>>> games =
      std::make_shared<std::vector<std::shared_ptr<PgnGame>>>();
  std::shared_ptr<OpeningTablebase> openingTablebase = std::make_shared<OpeningTablebase>();

  games->emplace_back(std::make_shared<PgnGame>());
  populate_starting_position(&(games->back()->m_position));
  games->back()->m_opening_tablebase = openingTablebase;
  bool reading_game_moves = false;
  int linecount = 0;

  // TODO (put the pgn file reading code into its own function, read_pgn_file or something)
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
        games->back()->m_opening_tablebase = openingTablebase;
        reading_game_moves = false;
      }
    }
  }
  // Remove the very last game, which is empty
  games->pop_back();

  // openingTablebase->serialize_tablebase(get_individual_tablebase_filepath(file_path, ".tb"));

  // print statistics about pgn processing
  auto clock_end = std::chrono::high_resolution_clock::now();
  print_pgn_processing_performance_summary(
      clock_start, clock_end, std::this_thread::get_id(),
      games->size(), openingTablebase->m_tablebase.size(), file_path);
}

void serialize_tablebase_extern(std::string file_path_prefix)
{
  uint16_t bucket = std::stoi(file_path_prefix.substr(file_path_prefix.rfind("/") + 1));
  masterTablebase.tablebases[bucket].serialize_tablebase(file_path_prefix + ".tb");
}

void serialize_all(std::string file_path_prefix)
{
  // /Users/vas/repos/matemancpp/dev_data/tablebase/master_tablebase/timestamp  /01.tb

  ThreadPool thread_pool = ThreadPool();
  for (int bucket = 0; bucket < MasterTablebase::TABLEBASE_SHARD_COUNT; bucket++)
  {

    std::stringstream ss;
    ss << file_path_prefix << "/" << std::setw(10) << std::setfill('0') << bucket;

    Task task = Task(&serialize_tablebase_extern, ss.str());
    thread_pool.add_task(task);
  }
  thread_pool.join_pool();
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

void start_pgn_processing_tasks()
{
  // PROCESS PGN FILES
  auto clock_start = std::chrono::high_resolution_clock::now();
  std::cout << std::endl
            << ColorCode::yellow << "Starting PGN processing..." << ColorCode::end << std::endl;

  std::filesystem::create_directories(master_tablebase_filepath);
  std::filesystem::create_directories(latest_individual_tablebases_filepath);
  std::filesystem::create_directories(latest_master_tablebase_filepath);
  std::set<std::string> completed_tablebases =
      OpeningTablebase::get_already_completed_tablebases_filenames(timestamp_of_attempt_to_use);

  ThreadPool thread_pool = ThreadPool();

  print_pgn_processing_header();

  for (const auto &entry : std::filesystem::directory_iterator(pgn_database_path))
  // for (const auto &entry : std::filesystem::directory_iterator("/Users/vas/repos/matemancpp/database/subtest3"))
  {

    std::string filepath = entry.path().generic_string();
    size_t path_end = filepath.rfind('/');
    size_t extension_start = filepath.rfind(".pgn");
    std::string filename = filepath.substr(path_end + 1, extension_start - path_end - 1);

    // if (completed_tablebases.find(filename) == completed_tablebases.end())
    // {
    Task task = Task(&process_pgn_file, entry.path());
    thread_pool.add_task(task);
    // }
  }
  thread_pool.join_pool();

  auto clock_end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start);
  std::cout << ColorCode::green << "ThreadPool has completed pgn processing tasks. " << ColorCode::end
            << "Elapsed time: " << duration.count() << " milliseconds." << std::endl;
  // -------------------------

  // SERIALIZE MASTER TABLEBASES
  auto clock_start4 = std::chrono::high_resolution_clock::now();
  std::cout << ColorCode::yellow << "Serializing tablebases..." << ColorCode::end << std::endl;

  serialize_all(latest_master_tablebase_filepath);

  auto clock_end4 = std::chrono::high_resolution_clock::now();
  auto duration4 = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end4 - clock_start4);
  std::cout << ColorCode::green << "Done serializing! " << ColorCode::end
            << "Elapsed time: " << duration4.count() << " milliseconds." << std::endl;
  // ----------------------------

  // READ TABLEBASES FROM DISK AND AGGREGATE
  /*
  auto clock_start3 = std::chrono::high_resolution_clock::now();
  std::cout << std::endl
            << ColorCode::yellow << "Reading tablebases from disk and aggregating them..." << ColorCode::end << std::endl;


  std::vector<std::shared_ptr<OpeningTablebase>> aggregated_tablebases =
      OpeningTablebase::aggregate_into_several_tablebases(latest_individual_tablebases_filepath);

  auto clock_end3 = std::chrono::high_resolution_clock::now();
  auto duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end3 - clock_start3);
  std::cout << ColorCode::green << "Done reading + aggregating! " << ColorCode::end
            << "Elapsed time: " << duration3.count() << " milliseconds." << std::endl;
            */
  // -----

  /*
  // READ TABLEBASES FROM DISK
  auto clock_start0 = std::chrono::high_resolution_clock::now();
  std::cout << std::endl
            << ColorCode::yellow << "Reading tablebases from disk..." << ColorCode::end << std::endl;

  std::vector<std::unique_ptr<OpeningTablebase>> deserialized_tablebases =
      OpeningTablebase::read_tablebases(latest_individual_tablebases_filepath);

  auto clock_end0 = std::chrono::high_resolution_clock::now();
  auto duration0 = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end0 - clock_start0);
  std::cout << ColorCode::green << "Done reading! " << ColorCode::end
            << "Elapsed time: " << duration0.count() << " milliseconds." << std::endl;
  // ----------------

  // MERGE TABLEBASES
  auto clock_start1 = std::chrono::high_resolution_clock::now();
  std::cout << std::endl
            << ColorCode::yellow << "Merging tablebases..." << ColorCode::end << std::endl;

  std::shared_ptr<OpeningTablebase> merged_tablebase = OpeningTablebase::merge_tablebases(&deserialized_tablebases);

  auto clock_end1 = std::chrono::high_resolution_clock::now();
  auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end1 - clock_start1);
  std::cout << ColorCode::green << "Done merging! " << ColorCode::end
            << "Elapsed time: " << duration1.count() << " milliseconds." << std::endl;
  // ----------------
  */

  // SERIALIZE MERGED TABLEBASE
  /*
  auto clock_start2 = std::chrono::high_resolution_clock::now();
  std::cout << ColorCode::yellow << "Serializing tablebases..." << ColorCode::end << std::endl;

  for (int i = 0; i < aggregated_tablebases.size(); i++)
  {
    aggregated_tablebases.at(i)->serialize_tablebase(latest_master_tablebase_filepath + '_' + std::to_string(i) + ".tb");
  }

  auto clock_end2 = std::chrono::high_resolution_clock::now();
  auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end2 - clock_start2);
  std::cout << ColorCode::green << "Done serializing! " << ColorCode::end
            << "Elapsed time: " << duration2.count() << " milliseconds." << std::endl;
            */
  // ----------------

  // POSITION HASH DISTRIBUTION
  /*
  long tablebase_position_hash_distribution_sum = 0;
  long tablebase_position_hash_hash_distribution_sum = 0;
  for (int i = 0; i < 64; i++)
  {
    tablebase_position_hash_distribution_sum += tablebase_position_hash_distribution[i];
    tablebase_position_hash_hash_distribution_sum += tablebase_position_hash_hash_distribution[i];
  }

  std::cout << ColorCode::yellow << "Position Hash distribution (0.015625 is even)" << ColorCode::end << std::endl;
  for (int i = 0; i < 64; i++)
  {
    std::cout
        << ColorCode::teal
        << std::left << std::setw(10) << (std ::to_string(i) + ":")
        << std::left << std::setw(16) << tablebase_position_hash_distribution[i]
        << std::left << std::setw(16) << (float)tablebase_position_hash_distribution[i] / tablebase_position_hash_distribution_sum
        << ColorCode::end << std::endl;
  }
  std::cout << std::endl;
  std::cout << ColorCode::yellow << "Position Hash Hash distribution (0.015625 is even)" << ColorCode::end << std::endl;
  for (int i = 0; i < 64; i++)
  {
    std::cout
        << ColorCode::teal
        << std::left << std::setw(10) << (std ::to_string(i) + ":")
        << std::left << std::setw(16) << tablebase_position_hash_hash_distribution[i]
        << std::left << std::setw(16) << (float)tablebase_position_hash_hash_distribution[i] / tablebase_position_hash_hash_distribution_sum
        << ColorCode::end << std::endl;
  }
  */
  // ----------------------

  std::cout
      << ColorCode::green << "Success!" << ColorCode::end << std::endl;
  exit(0);

  // print_game_summaries();
}

void read_all_pgn_files()
{
  std::set completed_files = get_completed_files_set();
  for (const auto &entry : std::filesystem::directory_iterator(pgn_database_path))
  {
    if (completed_files.find(entry.path()) != completed_files.end())
    {
      std::cout << "Skipping: " << entry.path() << std::endl;
      continue;
    }

    std::cout << "\u001b[31m " << entry.path() << "\u001b[0m" << std::endl;
    process_pgn_file(entry.path());

    // Update set of completed files
    std::cerr << "Completed: " << entry.path() << std::endl;
    update_completed_files_set(entry.path(), &completed_files);
  }
}
