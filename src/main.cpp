#include "cli.cpp"
#include "cli.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>

int main(int argc, char *argv[]) { cli_loop(); }

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