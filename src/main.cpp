#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

enum Command {
  uci,
  debug,
  isready,
  setoption,
  _register,
  ucinewgame,
  position,
  go,
  stop,
  ponderhit,
  quit
};

typedef void (*CommandProcessor)(std::vector<std::string>);
std::unordered_map<Command, CommandProcessor> command_processor_map;
std::unordered_map<std::string, Command> command_map;

void process_command_uci(std::vector<std::string> args) {
  std::cout << "id name MateMan" << std::endl;
  std::cout << "id author Vasia Patov" << std::endl;
}

void process_command_debug(std::vector<std::string> args) {
  std::cout << "Processing Debug!" << std::endl;
}

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

void cli_main_thread() {
  // Listen on stdin indefinitely
  // Process each command (for now just echo it with suffix)
  // Lightweight command processing can happen in this thread, heavy processing
  // should use other threads

  for (std::string line; std::getline(std::cin, line);) {
    process_command(line);
  }
}

int main(int argc, char *argv[]) {
  init_command_map();
  cli_main_thread();
}

/*
TODO
- Create remote repository (private)
- Install logging library, log to file
- Look into setoptions
    one option could be threadcount
- Create class for command processor
- Start working on basic chess engine implementation
- Separate application into several files
- read up on bitboards and consider trying them again
    (although, mailbox should be fine, the more interesting part is the
heuristic evaluation)

END GOALS:
- play via the arena UCI-friendly GUI
- make a computer that can beat you, and your friends
*/