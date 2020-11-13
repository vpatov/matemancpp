#ifndef __FEN_H__
#define __FEN_H__

#include "position.hpp"
#include <memory>

std::shared_ptr<Position> fen_to_position(std::string fen);

#endif