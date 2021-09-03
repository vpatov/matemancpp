#pragma once

#include "move_generation.hpp"
#include "position.hpp"
#include "tablebase.hpp"
#include "util.hpp"
#include "pgn_game.hpp"
#include <fstream>
#include <set>
#include <thread>
#include <boost/algorithm/string.hpp>

#define ELO_THRESHOLD 2200

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

void read_pgn_file(std::string file_path);
void read_all_pgn_files();
void start_pgn_processing_tasks();
