
#include "cli.hpp"
#include "fen.hpp"
#include <iostream>

int main(int argc, char *argv[]) { cli_loop(); }

/*
TODO
- Consider how to go about error handling (for invalid FEN strings for example)
- Create remote repository (private)
- Install logging library, log to file
- Look into setoptions
    one option could be threadcount

CURRENT GOALS:
- Implement minimum uci commands necessary to have barebones legal engine
- Successfully play 5 hardcoded moves from queens gambit line as white in UI,
connected to engine

MAIN GOALS:
- play via the arena UCI-friendly GUI
- make a computer that can beat you, and your friends
*/