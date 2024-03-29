#include "process_pgn/read_pgn_data.hpp"
#include "process_pgn/pgn_game.hpp"
#include "threadpool/threadpool.hpp"
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

std::shared_ptr<Tablebase> create_tablebases_from_pgn_data(std::string tablebase_name)
{
  PgnProcessor pgnProcessor(tablebase_data_dir / tablebase_name, pgn_database_path);
  pgnProcessor.process_pgn_files();
  return pgnProcessor.serialize_all();

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

  debugStream
      << ColorCode::green << "Success!" << ColorCode::end << std::endl;
}

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

  debugStream
      << std::left << std::setw(16) << thread_id
      << std::left << std::setw(16) << duration_str + "s"
      << std::left << std::setw(16) << games_list_size
      << std::left << std::setw(16) << (games_list_size / ((float)duration.count() / 1000))
      << std::left << std::setw(16) << tablebase_size
      << ColorCode::green
      << file_path.substr(file_path.find_last_of('/') + 1)
      << ColorCode::end
      << std::endl;
}
void print_pgn_processing_header()
{
  debugStream
      << "\u001b[33m"
      << std::left << std::setw(16) << "Thread ID"
      << std::left << std::setw(16) << "Duration"
      << std::left << std::setw(16) << "# of Games"
      << std::left << std::setw(16) << "Games / Second"
      << std::left << std::setw(16) << "Tablebase Size"
      << std::left << std::setw(16) << "File Name"
      << "\u001b[0m"
      << std::endl;
}

void hash_distribution()
{
  long tablebase_position_hash_distribution[Tablebase::get_shard_count()];
  long tablebase_position_hash_hash_distribution[Tablebase::get_shard_count()];
  for (int i = 0; i < Tablebase::get_shard_count(); i++)
  {
  }

  long tablebase_position_hash_distribution_sum = 0;
  long tablebase_position_hash_hash_distribution_sum = 0;
  for (int i = 0; i < Tablebase::get_shard_count(); i++)
  {
    tablebase_position_hash_distribution_sum += tablebase_position_hash_distribution[i];
    tablebase_position_hash_hash_distribution_sum += tablebase_position_hash_hash_distribution[i];
  }

  std::cout << ColorCode::yellow << "Position Hash distribution (0.015625 is even)" << ColorCode::end << std::endl;
  for (int i = 0; i < Tablebase::get_shard_count(); i++)
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
  for (int i = 0; i < Tablebase::get_shard_count(); i++)
  {
    std::cout
        << ColorCode::teal
        << std::left << std::setw(10) << (std ::to_string(i) + ":")
        << std::left << std::setw(16) << tablebase_position_hash_hash_distribution[i]
        << std::left << std::setw(16) << (float)tablebase_position_hash_hash_distribution[i] / tablebase_position_hash_hash_distribution_sum
        << ColorCode::end << std::endl;
  }
}