#pragma once
#include "representation/pieces.hpp"
#include "representation/squares.hpp"
#include <string>

square_t an_square_to_index(std::string square);
#define sq(s) an_square_to_index(s)
square_t an_square_to_index(char src_file, char src_rank);
piece_t char_to_piece(char piece);
char piece_to_char(piece_t piece);
const std::string piece_to_name(piece_t piece);
std::string piece_to_color_coded_char(piece_t piece, bool highlight);
std::string piece_to_unicode_char(piece_t piece);
std::string index_to_an_square(square_t index);
char index_to_an_file(square_t index);
char index_to_an_rank(square_t index);