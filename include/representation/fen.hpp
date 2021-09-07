#pragma once

#include "position.hpp"
#include <memory>

std::shared_ptr<Position> fen_to_position(std::string fen);
