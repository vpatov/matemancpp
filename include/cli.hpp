#ifndef __CLI_H__
#define __CLI_H__

#include <iostream>
#include <string>
#include <unistd.h>
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

void process_command_uci(std::vector<std::string> args);
void process_command_debug(std::vector<std::string> args);
void init_command_map();
void process_command(std::string command);
void cli_loop();

#endif
/*
 /home/vas/repos/matemancpp/src/cli.cpp:43:41: error:
 cannot convert
 ‘CLI::process_command_uci’ from type
 ‘void (CLI::)(std::vector<std::__cxx11::basic_string<char> >)’
 to type
 ‘void (*)    (std::vector<std::__cxx11::basic_string<char> >)’
*/