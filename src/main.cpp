
#include "cli.hpp"
#include "fen.hpp"
#include "move_generation.hpp"
#include "position.hpp"
#include "read_pgn_data.hpp"
#include "zobrist.hpp"
#include "threadpool.hpp"
#include <iostream>

int main(int argc, char *argv[])

{
   // test();
   read_all_pgn_files();
   // try_threading();
}

/*
    Tasks

    pgn parser - need to make multithreaded


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
- Install logging library, log to file
- Look into setoptions
    one option could be threadcount

- clean up header files so that foo.cpp and foo.hpp arent including the same
things
- clearly name types of squares, human-readable squares, indeces, etc
- create separate type for squares, and for pieces, such that they cannot be interchangeably used.
- add more assertions
- refactor code
   - smaller functions
   - better interfaces
- Make opening tablebase generation multi-threaded
- Make it work on linux (again)
- Create an OS-agnostic setup w.r.t file locations and Cmake files
- After finishing PGN parser, consider rethinking data structures for main chess engine.
   - Consider using bitboards and piece sets again
   - Think about memory model, work on design for what exactly will be going on 
      in the minimax position evaluation algorithm
   - Possible optimization: positional metadata (probably known as attack and defend sets), 
      such as a list of pieces that attack the king,
      can be stored on the position and saved to be used later, and can be made immutable
      and shared between positions (as long as it doesnt change).
   - Make move and unmake move - write these functions correctly and you don't have to do nearly 
    as much allocation during position search




CHAPTER 1: PGN READER + SERIALIZED OPENING TABLEBASE
CHAPTER 2: READ MORE CHESSPROGRAMMING WIKI
CHAPTER 3: RESEARCH SEARCH ALGORITHM (MINMAX) AND UNDERSTAND MEMORY MODEL
CHAPTER 4: REVISE DATA STRUCTURES AND BOARD/PIECE REPRESENTATION 
   (after research, and learnings from flaws of pgn reader approach)
CHAPTER 5: IMPLEMENT BASIC POSITION EVALUATION FUNCTION + MINMAX




CURRENT GOALS:
- Implement minimum uci commands necessary to have barebones legal engine
- Successfully play 5 hardcoded moves from queens gambit line as white in UI,
connected to engine
- Generate opening tablebase from PGN files.
- Have engine play as deep as it can from opening tablebase, picking moves at random,
  with the popularity of the move determining the probability.


MAIN GOALS:
- play via the arena UCI-friendly GUI
- make a computer that can beat you, and your friends
- engineer a readable and well-designed implementation that you are proud of (not a spaghetti mess)
- learn lots of C++ in the process

CONSIDERATIONS
==================
- Consider making opening tablebase in C++ and the engine in some other language?
   Pros:
   - Get to spend time learning another lang
   - Already spent time learning intuitions about how the engine should be done after writing the pgn parser

   Cons:
   - Will be spending more time learning another lang, and less time on the chess engine
   - Will make lots of beginner's mistake in the new lang and might feel like rewriting stuff
      after learning more.

*/