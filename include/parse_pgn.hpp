#ifndef __PARSE_PGN_H__
#define __PARSE_PGN_H__

#include <string>

void parse_pgn_game();
void parse_pgn_moves();
void parse_pgn_metadata();
void parse_pgn_file(std::string file_path);
void parse_all_pgn_files();
#endif