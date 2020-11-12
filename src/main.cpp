
#include "cli.hpp"
#include "fen.hpp"
#include "move_generation.hpp"
#include "position.hpp"
#include <iostream>

int main(int argc, char *argv[]) {

  std::shared_ptr<Position> position = starting_position();
  for (uint8_t square = 16; square <= 23; square++) {
    std::vector<uint8_t> moves =
        generate_pseudolegal_wpawn_moves(position, square);
    std::cout << "Moves for " << index_to_an_square(square) << ":" << std::endl;
    for (auto it = moves.begin(); it != moves.end(); it++) {
      std::cout << index_to_an_square(*it) << std::endl;
    }
    std::cout << std::endl;
  }
  cli_loop();
}

/*
    MAIN TASK
    Move generation
*/

/*
TODO
- Consider how to go about error handling (for invalid FEN strings for example)
- Create remote repository (private)
- Install logging library, log to file
- Look into setoptions
    one option could be threadcount

- clean up header files so that foo.cpp and foo.hpp arent including the same
things
- clearly name types of squares, human-readable squares, indeces, etc


CURRENT GOALS:
- Implement minimum uci commands necessary to have barebones legal engine
- Successfully play 5 hardcoded moves from queens gambit line as white in UI,
connected to engine

MAIN GOALS:
- play via the arena UCI-friendly GUI
- make a computer that can beat you, and your friends
*/