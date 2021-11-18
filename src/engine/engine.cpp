#include "engine/engine.hpp"

std::string Engine::find_best_move(std::chrono::milliseconds time)
{
    // try to look up position in opening tablebase. if found,
    // pick a move, with the likelihood of a move being selected proportional
    // to how often that move is seen.
    // return in long algebraic notation
    return "e2e4";
}