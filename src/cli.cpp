#include "cli.hpp"
#include "options.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

std::unordered_map<Command, CommandProcessor> command_processor_map;
std::unordered_map<std::string, Command> command_map;

void cli_loop() {
  init_command_map();

  for (std::string line; std::getline(std::cin, line);) {
    // spawn thread to handle command
    // if command is computationally expensive, command will
    // have its own threadpool
    std::thread command_thread(process_command, line);
    command_thread.detach();
  }
}

void announce_uciok() { std::cout << "uciok" << std::endl; }
void announce_readyok() { std::cout << "readyok" << std::endl; }

void process_command_uci(std::vector<std::string> args) {
  std::cout << "id name MateMan" << std::endl;
  std::cout << "id author Vasia Patov" << std::endl;
  announce_options();
  announce_uciok();
}
int hardcoded_stage = 0;
void hardcoded_response() {
  switch (hardcoded_stage) {
  case 0:
    std::cout << "bestmove g8f6" << std::endl;
    break;
  case 1:
    std::cout << "bestmove a7a6" << std::endl;
    break;
  case 2:
    std::cout << "bestmove a6a5" << std::endl;
    break;
  case 3:
    std::cout << "bestmove a5a4" << std::endl;
    break;
  case 4:
    std::cout << "bestmove h7h6" << std::endl;
    break;
  }
  hardcoded_stage++;
}

void process_command_debug(std::vector<std::string> args) {
  std::cout << "Processing Debug!" << std::endl;
  sleep(10);
  std::cout << "Done Debug!" << std::endl;
}

void process_command_isready(std::vector<std::string> args) {
  // TODO Determine if engine is actually ready (if it has finished setting all
  // options)
  announce_readyok();
};
void process_command_register(std::vector<std::string> args){};
void process_command_ucinewgame(std::vector<std::string> args){};
void process_command_position(std::vector<std::string> args){};
void process_command_go(std::vector<std::string> args) {
  hardcoded_response();
};
void process_command_stop(std::vector<std::string> args){};
void process_command_ponderhit(std::vector<std::string> args){};
void process_command_quit(std::vector<std::string> args) {
  // TODO cleanup?
  exit(0);
};

void init_command_map() {
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
}

void process_command(std::string command) {
  std::vector<std::string> args;
  boost::algorithm::split(args, command, boost::algorithm::is_any_of("\t "),
                          boost::token_compress_on);
  std::string primary = args.at(0);

  auto it = command_map.find(primary);
  if (it == command_map.end()) {
    // Error handling of invalid command
    return;
  }
  auto command_processor = command_processor_map.find(it->second)->second;
  command_processor(args);
}
