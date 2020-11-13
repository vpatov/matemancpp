
#include "cli.hpp"
#include "fen.hpp"
#include "move_generation.hpp"
#include "position.hpp"
#include <iostream>

int main(int argc, char *argv[]) {

  std::shared_ptr<Position> position = fen_to_position(
      "rnbqkbn1/pp1pp1p1/1P5r/8/2R2pp1/7P/2PPPPP1/1NBQKBNR w Kq - 0 1");

  uint8_t square = an_square_to_index("c4");
  std::vector<uint8_t> moves =
      generate_pseudolegal_rook_moves<Color ::WHITE>(position, square);
  std::cout << "Moves for " << index_to_an_square(square) << ":" << std::endl;
  for (auto it = moves.begin(); it != moves.end(); it++) {
    std::cout << index_to_an_square(*it) << std::endl;
  }
  std::cout << std::endl;

  square = an_square_to_index("h6");
  moves = generate_pseudolegal_rook_moves<Color ::BLACK>(position, square);
  std::cout << "Moves for " << index_to_an_square(square) << ":" << std::endl;
  for (auto it = moves.begin(); it != moves.end(); it++) {
    std::cout << index_to_an_square(*it) << std::endl;
  }
  std::cout << std::endl;

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