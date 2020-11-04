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

std::unordered_map<std::string, Command> command_map;

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
}

void process_command(std::string command) {
  std::vector<std::string> tokenized;
  boost::algorithm::split(tokenized, command,
                          boost::algorithm::is_any_of("\t "),
                          boost::token_compress_on);
  std::string primary = tokenized.at(0);
  std::cerr << "Received command: " << primary << std::endl;
  for (std::vector<std::string>::iterator t = ++tokenized.begin();
       t != tokenized.end(); ++t) {
    std::cout << *t << std::endl;
  }

  // std::cout << command.size() << std::endl;
}

void cli_main_thread() {
  // Listen on stdin indefinitely
  // Process each command (for now just echo it with suffix)
  // Lightweight command processing can happen in this thread, heavy processing
  // should use other threads

  for (std::string line; std::getline(std::cin, line);) {
    process_command(line);
    // std::cout << line << std::endl;
  }
}

int main(int argc, char *argv[]) { cli_main_thread(); }