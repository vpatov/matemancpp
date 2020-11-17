
#include "cli.hpp"
#include "fen.hpp"
#include "move_generation.hpp"
#include "parse_pgn.hpp"
#include "position.hpp"
#include "zobrist.hpp"
#include <iostream>

int main(int argc, char *argv[]) { parse_all_pgn_files(); }

/*
    Tasks

    - Castling
    - Legal move generation
      - Move cannot cause mover's king to be in check
      - Mover's king cannot be in check, and cannot pass through check during
    - Move representation (i.e. e2e4 or [16,32])
    - Opening book (for up to 4/5 moves) https://www.chessprogramming.org/ECO
    - endgame tablebases
   castling
   - Position Evaluation Function
   - Chess Engine that plays random legal moves, and is rule complete.

Another drawback is that tablebases require a lot of memory to store the many
thousands of positions. The Nalimov tablebases, which use advanced compression
techniques, require 7.05 GB of hard disk space for all 5-piece endings. The
6-piece endings require approximately 1.2 TB.[39][40] The 7-piece Lomonosov
tablebase requires 140 TB of storage space.[41]

*/

/*
 Methods for finding legal moves from pseudolegal moves

    Naive:    For every move, iterate through opponents piece list and see if
 they are attacking the king

    Slightly better: For every move, check diagonals for enemy bishops/queens,
 files/ranks for enemy rooks/queens, knight-hops for enemy knights,
 pawn-captures for enemy pawns, and king captures for enemy kings. Then you
 don't have to maintain a piece list.

    Start with naive.
    If it's really slow then user a profiler to improve it.


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