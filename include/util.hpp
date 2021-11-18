#pragma once

#include <regex>
#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

const fs::path project_root_dir = (PROJECT_ROOT_DIR); // PROJECT_ROOT_DIR is defined in CMakeLists.txt
const fs::path dev_data_dir = project_root_dir / "dev_data";
const std::string program_start_timestamp = std::to_string(std::chrono::seconds(std::time(NULL)).count());

const fs::path pgn_database_path = fs::path(PROJECT_ROOT_DIR) / "database" / "pgn";
// const fs::path pgn_database_path = fs::path(PROJECT_ROOT_DIR) / "database" / "subtest";
const fs::path tablebase_data_dir = dev_data_dir / "tablebase";
const fs::path master_tablebase_data_dir = tablebase_data_dir / "master_tablebase";
const fs::path completed_files_filepath = dev_data_dir / "completed_files.txt";

struct Move
{
    uint8_t src_square;
    uint8_t dest_square;
    uint8_t promotion_piece;

    friend std::ostream &operator<<(std::ostream &os, Move &move);
};

namespace ColorCode
{
    static const std::string white = "\u001b[37m";
    static const std::string teal = "\u001b[36m";
    static const std::string purple = "\u001b[35m";
    static const std::string blue = "\u001b[34m";
    static const std::string yellow = "\u001b[33m";
    static const std::string green = "\u001b[32m";
    static const std::string red = "\u001b[31m";
    static const std::string end = "\u001b[0m";
};

Move unpack_move_key(uint32_t move_key);
char getc(int i, std::smatch &matches);
