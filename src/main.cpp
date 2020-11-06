
#include "cli.hpp"
#include "fen.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>

// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
int main(int argc, char *argv[]) {
  auto position = fen_to_position("8/8/8/8/8/r7/b7/kb5K w - - 0 1");

  print_position(position);

  // cli_loop();
}

/*
TODO
- Create remote repository (private)
- Install logging library, log to file
- Look into setoptions
    one option could be threadcount
- Start working on basic chess engine implementation
- Separate application into several files
- read up on bitboards and consider trying them again
    (although, mailbox should be fine, the more interesting part is the
heuristic evaluation)

END GOALS:
- play via the arena UCI-friendly GUI
- make a computer that can beat you, and your friends
*/