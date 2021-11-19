#pragma once

#include <iostream>
#include <string>
#include <unistd.h>

#include <unordered_map>
#include <vector>

#include "tablebase/master_tablebase.hpp"
#include "engine/engine.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
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
  read_tablebases,
  test_tablebases
};

class CLI
{
public:
  std::shared_ptr<MasterTablebase> m_master_tablebase;
  Engine m_engine;
  spdlog::logger m_logger;

  spdlog::logger create_logger()
  {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);
    console_sink->set_pattern("[%^%l%$] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
    file_sink->set_level(spdlog::level::info);

    auto logger = spdlog::logger("multi_sink", {console_sink, file_sink});

    logger.set_level(spdlog::level::info);
    logger.info("this message should not appear in the console, only in the file");
    spdlog::flush_every(std::chrono::seconds(1));

    return logger;
  }

  CLI() : m_logger(create_logger())
  {
    // try
    // {

    // std::vector<spdlog::sink_ptr> sinks;
    // sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
    // logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    // }
    // catch (const spdlog::spdlog_ex &ex)
    // {
    //   std::cout << "Log init failed: " << ex.what() << std::endl;
    //   throw ex;
    //   exit(1);
    // }
  }

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
  void process_command_test_tablebases(std::vector<std::string> args);

  void init_command_map();
  void process_command(std::string command);
  void cli_loop();
  void log_and_respond(std::string output);
  void hardcoded_response();
  void announce_readyok();
  void announce_uciok();
};
using CommandProcessor = void (CLI::*)(std::vector<std::string>);
