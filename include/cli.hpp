#pragma once

#include <iostream>
#include <string>
#include <unistd.h>

#include <unordered_map>
#include <vector>

enum Command
{
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
  quit,
  create_tablebases,
  read_tablebases
};
typedef void (*CommandProcessor)(std::vector<std::string>);

void process_command_uci(std::vector<std::string> args);
void process_command_debug(std::vector<std::string> args);
void process_command_isready(std::vector<std::string> args);
void process_command_register(std::vector<std::string> args);
void process_command_ucinewgame(std::vector<std::string> args);
void process_command_position(std::vector<std::string> args);
void process_command_go(std::vector<std::string> args);
void process_command_stop(std::vector<std::string> args);
void process_command_ponderhit(std::vector<std::string> args);
void process_command_quit(std::vector<std::string> args);
void process_command_create_tablebases(std::vector<std::string> args);
void process_command_read_tablebases(std::vector<std::string> args);

void init_command_map();
void process_command(std::string command);
void cli_loop();
