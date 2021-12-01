#pragma once

#include "move_generation.hpp"
#include "representation/position.hpp"
#include "threadpool/threadpool.hpp"
#include "util.hpp"
#include "pgn_game.hpp"
#include "tablebase/tablebase.hpp"
#include <filesystem>
#include <fstream>
#include <set>
#include <thread>
#include <boost/algorithm/string.hpp>

#define ELO_THRESHOLD 2200

const bool debug_disabled = true;
#define debugStream     \
    if (debug_disabled) \
    {                   \
    }                   \
    else                \
        std::cerr

namespace fs = std::filesystem;

const std::string result_regex_str =
    R"((((?:1\/2|1|0)\s*\-\s*(?:1\/2|1|0)\s*$)|\*)?)";
const std::regex game_line_regex(
    R"(\d+\.\s*([\w\-\+\#\=]+)\s([\w\-\+\#\=]+)?\s*)" + result_regex_str);
const std::string metadata_line_regex =
    "^\\s*\\[(\\w+)\\s\"(.*?)\"\\]\\s*$";
const std::string non_castling_move_regex =
    "([RNBKQ])?([a-h])?([1-8])?(x)?([a-h])"
    "([1-8])(=[RNBKQ])?([\\+\\#])?";
const std::string castling_move_regex = "((O-O-O)|(O-O))([\\+\\#])?";

std::shared_ptr<Tablebase> create_tablebases_from_pgn_data(std::string tablebase_name);

void print_pgn_processing_performance_summary(
    std::__1::chrono::steady_clock::time_point clock_start,
    std::__1::chrono::steady_clock::time_point clock_end,
    std::thread::id thread_id,
    int games_list_size,
    int tablebase_size,
    std::string file_path);
void print_pgn_processing_header();

class PgnProcessor
{
    std::shared_ptr<Tablebase> m_tablebase;
    fs::path m_tablebase_destination_file_path;
    fs::path m_pgn_database_path;

public:
    PgnProcessor(std::string tablebase_destination_file_path, std::string pgn_database_path)
        : m_tablebase_destination_file_path(tablebase_destination_file_path), m_pgn_database_path(pgn_database_path)
    {
        m_tablebase = std::make_shared<Tablebase>();
    }

    std::shared_ptr<Tablebase> get_tablebase()
    {
        return m_tablebase;
    }

    std::shared_ptr<Tablebase> serialize_all()
    {
        auto clock_start = std::chrono::high_resolution_clock::now();
        debugStream << ColorCode::yellow << "Serializing tablebases..." << ColorCode::end << std::endl;

        m_tablebase->serialize_all(m_tablebase_destination_file_path);

        auto clock_end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start);
        debugStream << ColorCode::green << "Done serializing! " << ColorCode::end
                    << "Elapsed time: " << duration.count() << " milliseconds." << std::endl;

        return m_tablebase;
    }

    void process_pgn_files()
    {
        auto clock_start = std::chrono::high_resolution_clock::now();
        debugStream << std::endl
                    << ColorCode::yellow << "Starting PGN processing..." << ColorCode::end << std::endl;

        ThreadPool thread_pool = ThreadPool();
        print_pgn_processing_header();

        std::function<void(std::string &)> process_pgn_file_bound_fn =
            std::bind(&PgnProcessor::process_pgn_file, this, std::placeholders::_1);

        for (const auto &entry : std::filesystem::directory_iterator(m_pgn_database_path))
        {
            std::string filepath = entry.path().generic_string();
            size_t path_end = filepath.rfind('/');
            size_t extension_start = filepath.rfind(".pgn");
            std::string filename = filepath.substr(path_end + 1, extension_start - path_end - 1);

            Task task = Task(&process_pgn_file_bound_fn, entry.path());
            thread_pool.add_task(task);
        }
        thread_pool.join_pool();

        auto clock_end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start);
        debugStream << ColorCode::green << "ThreadPool has completed pgn processing tasks. " << ColorCode::end
                    << "Elapsed time: " << duration.count() << " milliseconds." << std::endl;
    }

    void process_pgn_file(std::string file_path)
    {
        auto clock_start = std::chrono::high_resolution_clock::now();
        std::ifstream infile(file_path);
        std::shared_ptr<std::vector<std::shared_ptr<PgnGame>>> games =
            std::make_shared<std::vector<std::shared_ptr<PgnGame>>>();

        games->emplace_back(std::make_shared<PgnGame>());
        populate_starting_position(&(games->back()->m_position));
        bool reading_game_moves = false;
        int linecount = 0;

        // TODO (put the pgn file reading code into its own function, read_pgn_file or something)
        if (!infile.is_open())
        {
            debugStream << "Could not open " << file_path << std::endl;
            return;
        }
        for (std::string line; getline(infile, line);)
        {
            linecount++;
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

                bool is_game_line = games->back()->read_game_move_line(line, m_tablebase.get());
                if (games->back()->m_finishedReading)
                {

                    games->back()->populateMetadata();
                    // push a new game to the back of the games vector
                    games->emplace_back(std::make_shared<PgnGame>());
                    populate_starting_position(&(games->back()->m_position));
                    reading_game_moves = false;
                }
            }
        }
        // Remove the very last game, which is empty
        games->pop_back();

        // print statistics about pgn processing
        auto clock_end = std::chrono::high_resolution_clock::now();
        print_pgn_processing_performance_summary(
            clock_start, clock_end, std::this_thread::get_id(),
            games->size(), m_tablebase->total_size(), file_path);
    }
};