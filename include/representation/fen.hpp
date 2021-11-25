#pragma once

#include "position.hpp"
#include <memory>

std::shared_ptr<Position> fen_to_position(std::string fen);
std::string position_to_fen(std::shared_ptr<Position> position);