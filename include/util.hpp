#pragma once

#include <regex>
#include <iostream>

struct Move
{
    uint8_t src_square;
    uint8_t dest_square;
    uint8_t promotion_piece;

    friend std::ostream &operator<<(std::ostream &os, Move &move);
};

Move unpack_move_key(uint32_t move_key);
uint32_t generate_move_key(uint8_t src_square, uint8_t dest_square, uint8_t promotion_piece);
char getc(int i, std::smatch &matches);
