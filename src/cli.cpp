#include "cli.hpp"
#include "options.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include "representation/position.hpp"
#include <vector>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

std::unordered_map<Command, CommandProcessor> command_processor_map;
std::unordered_map<std::string, Command> command_map;

std::shared_ptr<spdlog::logger> logger;
std::shared_ptr<Position> current_position;

int hardcoded_stage = 0;

void log_and_respond(std::string output)
{
  logger->info("--> {}", output);
  std::cout << output << std::endl;
}

void cli_loop()
{
  try
  {
    logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    spdlog::flush_every(std::chrono::seconds(1));
  }
  catch (const spdlog::spdlog_ex &ex)
  {
    std::cout << "Log init failed: " << ex.what() << std::endl;
    exit(1);
  }

  logger->info("Starting main clip loop...");
  init_command_map();

  for (std::string line; std::getline(std::cin, line);)
  {

    logger->info("<-- {}", line);

    // spawn thread to handle command
    // if command is computationally expensive, command will
    // have its own threadpool
    process_command(line);
  }
}

void announce_uciok() { log_and_respond("uciok"); }
void announce_readyok() { log_and_respond("readyok"); }

void process_command_uci(std::vector<std::string> args)
{
  log_and_respond("id name MateMan");
  log_and_respond("id author Vasia Patov");
  announce_options();
  announce_uciok();
}
void hardcoded_response()
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

void process_command_debug(std::vector<std::string> args)
{
  std::cout << "Processing Debug!" << std::endl;
  sleep(10);
  std::cout << "Done Debug!" << std::endl;
}

void process_command_isready(std::vector<std::string> args)
{
  // TODO Determine if engine is actually ready (if it has finished setting all
  // options)
  announce_readyok();
};
void process_command_register(std::vector<std::string> args){};
void process_command_ucinewgame(std::vector<std::string> args){};

// position startpos moves c2c4
// position [fen <fenstring> | startpos ]  moves <move1> .... <movei>
void process_command_position(std::vector<std::string> args)
{
  if (!args.size())
  {
    logger->warn("No arguments for position command.");
    return;
  }

  auto it = args.begin();
  assert((it++)->compare("position") == 0);
  std::string startpos_type = *it++;
  if (startpos_type.compare("startpos") == 0)
  {
    // init position to startpos
    current_position = starting_position();
  }
  else if (startpos_type.compare("fen") == 0)
  {
    // init position from the fenstring
    std::string fen_string = *it++;
  }
  else
  {
    logger->warn("Unrecognized command for position: {}", startpos_type);
    return;
  }

  // either except moves move1 move2 ... moveN, or nothing
  if (it == args.end())
  {
    logger->debug("No moves found, using current position.");
    return;
  }
  std::string moves_str = *it++;
  if (moves_str.compare("moves") == 0)
  {
    // for each move, adjust the position with it.
    while (it != args.end())
    {
      std::string move = *it++;
      logger->debug("parsing move: {}", move);
      square_t src_square = an_square_to_index(move.substr(0, 2));
      square_t dst_square = an_square_to_index(move.substr(2, 4));

      current_position->adjust_position(src_square, dst_square, 0, 0);
      current_position->print_with_borders_highlight_squares(src_square, dst_square);
    }
  }
  else
  {
    logger->warn("Unrecognized continuation of position command: {}", moves_str);
  }
}
void process_command_go(std::vector<std::string> args)
{
  hardcoded_response();
};
void process_command_stop(std::vector<std::string> args){};
void process_command_ponderhit(std::vector<std::string> args){};
void process_command_quit(std::vector<std::string> args)
{
  // TODO cleanup?
  exit(0);
}

void process_command_create_tablebases(std::vector<std::string> args)
{
  std::string user_input;
  std::cout << "Input the name for this tablebase (leave blank to use the current timestamp):" << std::endl;
  std::getline(std::cin, user_input);

  if (user_input.size())
  {
  }
  std::cout << "you put in :{" << user_input << "}" << std::endl;

  exit(1);
  // start_pgn_processing_tasks();
}

void init_command_map()
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

  command_processor_map[Command::uci] = &process_command_uci;
  command_processor_map[Command::debug] = &process_command_debug;
  command_processor_map[Command::isready] = &process_command_isready;
  command_processor_map[Command::_register] = &process_command_register;
  command_processor_map[Command::ucinewgame] = &process_command_ucinewgame;
  command_processor_map[Command::position] = &process_command_position;
  command_processor_map[Command::go] = &process_command_go;
  command_processor_map[Command::stop] = &process_command_stop;
  command_processor_map[Command::ponderhit] = &process_command_ponderhit;
  command_processor_map[Command::quit] = &process_command_quit;
  command_processor_map[Command::create_tablebases] = &process_command_create_tablebases;
}

void process_command(std::string command)
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
  auto command_processor = command_processor_map.find(it->second)->second;

  // if the command is one of the ones that requires user input, dont spawn a new thread for it
  // so that it's easier to get user input.
  if (primary.compare("create_tablebases") == 0)
  {
    command_processor(args);
  }
  else
  {
    std::thread command_thread(command_processor, args);
    command_thread.detach();
  }
}
