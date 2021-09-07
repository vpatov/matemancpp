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
