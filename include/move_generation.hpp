#ifndef __MOVE_GENERATION_H__
#define __MOVE_GENERATION_H__
#include "position.hpp"
#include <cstdint>
#include <vector>

std::vector<uint8_t>
generate_pseudolegal_wpawn_moves(std::shared_ptr<Position> position,
                                 uint8_t square);

#endif