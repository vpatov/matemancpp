#ifndef __UTIL_H__
#define __UTIL_H__

#include <regex>

uint32_t generate_move_key(uint8_t src_square, uint8_t dest_square, uint8_t promotion_piece);
char getc(int i, std::smatch &matches);

#endif
