#include "cli.hpp"
#include "options.hpp"
#include "process_pgn/read_pgn_data.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include "representation/position.hpp"
#include <vector>

#include "engine/engine.hpp"

std::unordered_map<Command, CommandProcessor> command_processor_map;
std::unordered_map<std::string, Command> command_map;

int hardcoded_stage = 0;

void CLI::log_and_respond(std::string output)
{
  m_logger.info("--> {}", output);
  std::cout << output << std::endl;
}

void CLI::cli_loop()
{
  m_logger.info("Starting main clip loop...");
  init_command_map();

  for (std::string line; std::getline(std::cin, line);)
  {

    m_logger.info("<-- {}", line);

    // spawn thread to handle command
    // if command is computationally expensive, command will
    // have its own threadpool
    process_command(line);
  }
}

void CLI::announce_uciok() { log_and_respond("uciok"); }
void CLI::announce_readyok() { log_and_respond("readyok"); }

void CLI::process_command_uci(std::vector<std::string> args)
{
  log_and_respond("id name MateMan");
  log_and_respond("id author Vasia Patov");
  announce_options();
  announce_uciok();
}
void CLI::hardcoded_response()
{
  switch (hardcoded_stage)
  {
  case 0:
    // ponder("bestmove g8f6");

    std::this_thread::sleep_for(std::chrono::seconds(3));

    log_and_respond("bestmove g8f6");
    break;
  case 1:
    std::this_thread::sleep_for(std::chrono::seconds(3));

    log_and_respond("bestmove a7a6");
    break;
  case 2:
    std::this_thread::sleep_for(std::chrono::seconds(3));

    log_and_respond("bestmove a6a5");
    break;
  case 3:
    std::this_thread::sleep_for(std::chrono::seconds(3));

    log_and_respond("bestmove a5a4");
    break;
  case 4:
    std::this_thread::sleep_for(std::chrono::seconds(3));

    log_and_respond("bestmove h7h6");
  case 5:
    std::this_thread::sleep_for(std::chrono::seconds(3));

    log_and_respond("bestmove h6h5");
  case 6:
    log_and_respond("bestmove h5h4");
    break;
  }
  hardcoded_stage++;
}

void CLI::process_command_debug(std::vector<std::string> args)
{
  std::cout << "Processing Debug!" << std::endl;
  sleep(10);
  std::cout << "Done Debug!" << std::endl;
}

void CLI::process_command_isready(std::vector<std::string> args)
{
  // TODO Determine if engine is actually ready (if it has finished setting all
  // options)
  announce_readyok();
};
void CLI::process_command_register(std::vector<std::string> args){};
void CLI::process_command_ucinewgame(std::vector<std::string> args){};

// position startpos moves c2c4
// position [fen <fenstring> | startpos ]  moves <move1> .... <movei>
void CLI::process_command_position(std::vector<std::string> args)
{
  if (!args.size())
  {
    m_logger.warn("No arguments for position command.");
    return;
  }

  auto it = args.begin();
  assert((it++)->compare("position") == 0);
  std::string startpos_type = *it++;
  if (startpos_type.compare("startpos") == 0)
  {
    // init position to startpos
    m_engine.m_current_position = starting_position();
  }
  else if (startpos_type.compare("fen") == 0)
  {
    // init position from the fenstring
    std::string fen_string = *it++;
  }
  else
  {
    m_logger.warn("Unrecognized command for position: {}", startpos_type);
    return;
  }

  // either except moves move1 move2 ... moveN, or nothing
  if (it == args.end())
  {
    m_logger.debug("No moves found, using current position.");
    return;
  }
  std::string moves_str = *it++;
  if (moves_str.compare("moves") == 0)
  {
    // for each move, adjust the position with it.
    while (it != args.end())
    {
      std::string move = *it++;
      m_logger.debug("parsing move: {}", move);
      square_t src_square = an_square_to_index(move.substr(0, 2));
      square_t dst_square = an_square_to_index(move.substr(2, 4));

      m_engine.m_current_position->advance_position2(src_square, dst_square, 0);
      // m_engine.m_current_position->print_with_borders_highlight_squares(src_square, dst_square);
    }
  }
  else
  {
    // TODO also configure logger to log to stderr
    m_logger.warn("Unrecognized continuation of position command: {}", moves_str);
  }
}

// LASTLEFTOFF implement go, position commands and test engine with moves picked from tablebase.

void CLI::process_command_go(std::vector<std::string> args)
{
  std::string best_move;

  // 	* movetime <x>
  // 	search exactly x mseconds
  // * infinite
  // 	search until the "stop" command. Do not exit the search without being told so in this mode!
  // write function that creates vector of pairs for keys/values
  int x = 1;
  auto time = std::chrono::milliseconds(5000);
  //movetime
  if (x == 1)
  {
    // assume pos is already set up
    best_move = m_engine.find_best_move(time);
    // std::this_thread::sleep_for(time);
  }
  // infinite
  else if (x == 2)
  {
    best_move = m_engine.find_best_move(std::chrono::milliseconds(0));
  }
  std::cout << "bestmove " << best_move << std::endl;

  // hardcoded_response();
};
void CLI::process_command_stop(std::vector<std::string> args)
{

  // * stop
  // 	stop calculating as soon as possible,
  // 	don't forget the "bestmove" and possibly the "ponder" token when finishing the search

  // interrupt thread that is calculating moves, if there is a thread that is doing that.
  // that thread should have been maintaining what the best move was at every given moment.
  // that thread should have some sort of an interrupt handler.
  // look into thread signaling or something
}
void CLI::process_command_ponderhit(std::vector<std::string> args){};
void CLI::process_command_quit(std::vector<std::string> args)
{
  // TODO cleanup?
  exit(0);
}

void CLI::process_command_create_tablebases(std::vector<std::string> args)
{

  if (args.size() < 2)
  {
    m_logger.info("You must provide a tablebase name");
    return;
  }
  std::string tablebase_name = args.at(1);

  if (!tablebase_name.size())
  {
    tablebase_name = program_start_timestamp;
  }
  else
  {
    // check tablebase name to make sure there are no illegal characters.
  }
  m_logger.debug("tablebase name: {}", tablebase_name);
  m_engine.set_tablebase(create_tablebases_from_pgn_data(tablebase_name));
}

void CLI::process_command_read_tablebases(std::vector<std::string> args)
{
  if (args.size() < 2)
  {
    std::cout << "You must provide a table name" << std::endl;
    return;
  }
  std::string tablebase_name = args.at(1);

  m_logger.debug("tablebase name: {}", tablebase_name);

  if (std::filesystem::is_directory(master_tablebase_data_dir / tablebase_name))
  {

    m_engine.set_tablebase(std::make_shared<Tablebase>(master_tablebase_data_dir / tablebase_name));
  }
  else
  {
    std::cerr << ColorCode::red << "Cannot find tablebase with name \"" << tablebase_name
              << ColorCode::end << std::endl;
  }
}

void CLI::process_command_test_tablebases(std::vector<std::string> args)
{
  m_engine.m_master_tablebase->walk_down_most_popular_path();
}

void CLI::init_command_map()
{
  command_map["uci"] = Command::uci;
  command_map["debug"] = Command::debug;
  command_map["isready"] = Command::isready;
  command_map["register"] = Command::_register;
  command_map["ucinewgame"] = Command::ucinewgame;
  command_map["position"] = Command::position;
  command_map["go"] = Command::go;
  command_map["stop"] = Command::stop;
  command_map["ponderhit"] = Command::ponderhit;
  command_map["quit"] = Command::quit;
  command_map["create_tablebases"] = Command::create_tablebases;
  command_map["read_tablebases"] = Command::read_tablebases;
  command_map["test_tablebases"] = Command::test_tablebases;

  command_processor_map[Command::uci] = &CLI::process_command_uci;
  command_processor_map[Command::debug] = &CLI::process_command_debug;
  command_processor_map[Command::isready] = &CLI::process_command_isready;
  command_processor_map[Command::_register] = &CLI::process_command_register;
  command_processor_map[Command::ucinewgame] = &CLI::process_command_ucinewgame;
  command_processor_map[Command::position] = &CLI::process_command_position;
  command_processor_map[Command::go] = &CLI::process_command_go;
  command_processor_map[Command::stop] = &CLI::process_command_stop;
  command_processor_map[Command::ponderhit] = &CLI::process_command_ponderhit;
  command_processor_map[Command::quit] = &CLI::process_command_quit;
  command_processor_map[Command::create_tablebases] = &CLI::process_command_create_tablebases;
  command_processor_map[Command::read_tablebases] = &CLI::process_command_read_tablebases;
  command_processor_map[Command::test_tablebases] = &CLI::process_command_test_tablebases;
}

void CLI::process_command(std::string command)
{
  std::vector<std::string> args;
  boost::algorithm::split(args, command, boost::algorithm::is_any_of("\t "),
                          boost::token_compress_on);
  std::string primary = args.at(0);

  auto it = command_map.find(primary);
  if (it == command_map.end())
  {
    // Error handling of invalid command
    std::cout << "Could not interpret command " << command << std::endl;
    return;
  }
  CommandProcessor command_processor = command_processor_map.find(it->second)->second;

  // if the command is one of the ones that requires user input, dont spawn a new thread for it
  // so that it's easier to get user input.
  if (primary.compare("create_tablebases") == 0 || primary.compare("read_tablebases") == 0)
  {
    (this->*command_processor)(args);
  }
  else
  {
    std::thread command_thread(command_processor, this, args);
    command_thread.detach();
  }
}
