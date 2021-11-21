#include "process_pgn/read_pgn_data.hpp"
#include "cli.hpp"
#include "representation/position.hpp"
#include "tablebase/zobrist.hpp"
#include <unordered_map>

int main(int argc, char *argv[])

{

   CLI cli;
   cli.cli_loop();
}
/*
High-level Design.

main server thread waits for user input, and asks the engine to do stuff.
server should start it's own thread for engine tasks, engine operation shouldnt block CLI.
Engine should manage its own threadpool.

Things that the server can do:
Can ask the PgnProcessor to create_tablebases from pgn files.
Can ask the engine to load tablebases from disk into memory, for use.
Can ask the engine to set up the current position
Can ask the engine to find the best move (given the engine's position)
Can ask the engine to stop searching, and report back the best thing it's found so far
Can ask the engine to set options

Think of CLI as server, and of the different commands as endpoints.
Looking at it this way, it's much clearer that the server owns the engine.

----------

Tablebase class needs to be consolidated into something that makes sense.
It's pretty confusing right now with MasterTablebase, OpeningTablebase, m_tablebases....
Would be good to just have one class with one interface for all tablebase interaction. 
and just call it Tablebase.
*/

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
- Look into setoptions
    one option could be threadcount

- clean up header files so that foo.cpp and foo.hpp arent including the same
things
- add more assertions
- refactor code
   - smaller functions
   - better interfaces
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

/*

  CLI cli;

   cli.process_command_uci({});
   cli.process_command_create_tablebases({"create_tablebases", "debug_002"});

   std::vector<std::string> args = {"test_tablebases", "test"};
   // cli.process_command_read_tablebases(args);
   cli.process_command_position({"position", "startpos"});

   auto position = starting_position();
   std::cout << "starting_position:\n"
             << zobrist_hash(position.get()) << std::endl;

   square_t src1 = an_square_to_index("e2");
   square_t dst1 = an_square_to_index("e4");
   position->advance_position(src1, dst1, 0, 36);

   std::cout << "position after e2e4:\n"
             << zobrist_hash(position.get()) << std::endl;

   cli.process_command_position({"position", "startpos", "moves", "e2e4"});

   auto move_map = (*cli.m_engine.m_master_tablebase)[zobrist_hash(starting_position().get())];

   for (auto it = move_map->begin(); it != move_map->end(); it++)
   {
      auto move_key = it->first;
      auto move_edge = it->second;
      std::cout << "---" << std::endl;
      std::cout << move_edge.m_dest_hash << " - "
                << move_edge.m_pgn_move << " - " << move_edge.m_times_played << std::endl;
      // std::cout << it->first << ", " << it->second
   }
   std::cout << "======================" << std::endl;

   auto move_map2 = (*cli.m_engine.m_master_tablebase)[1113003689047388558];

   for (auto it = move_map2->begin(); it != move_map2->end(); it++)
   {
      auto move_key = it->first;
      auto move_edge = it->second;
      std::cout << "---" << std::endl;
      std::cout << move_edge.m_dest_hash << " - "
                << move_edge.m_pgn_move << " - " << move_edge.m_times_played << std::endl;
      // std::cout << it->first << ", " << it->second
   }

   // for (auto it = tablebase.begin(); it != node.m_tablebase.end(); it++)
   // {
   //    auto first = it->first;
   //    auto second = it->second;
   // }

   // PROBLEM is because the pgn processing code is making changes to the position
   // the adjust method doesnt perform the complete change to the position, with things
   // such as plies and turn to move.....
   // also en passant square is calculated by pgn processing and then passed to adjust method.
   // bad design

   // TODO refactor interface to position?
   // problem is that you are using adjust_position for two purposeS:
   //    - to temporarily assume a position to check for legality
   //    - to actually change the position and move forward
   // also, advance positino shouldnt be taking en_passant_square as an argument.
   std::cout
       << "position exists? "
       << cli.m_engine.m_master_tablebase->position_exists(
              zobrist_hash(cli.m_engine.m_current_position.get()))
       << std::endl;
   std::cout << "position exists? "
             << cli.m_engine.m_master_tablebase->position_exists(
                    zobrist_hash(position.get()))
             << std::endl;

   // std::string user_input;
   // std::cout << "Input the name for this tablebase (leave blank to use the current timestamp):" << std::endl;
   // std::cin >> user_input;
   // std::cout << "foo:{" << user_input << "}" << std::endl;
   // exit(1);

   // start_pgn_processing_tasks();
   // cli.cli_loop();

   */