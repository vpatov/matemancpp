#pragma once

#include "tablebase.hpp"

struct metadata_entry
{
    std::string key;
    std::string value;
};

struct Game
{
    std::vector<metadata_entry> metadata;
    Position position;
    int whiteElo;
    int blackElo;
    bool eloOverThreshold;
    bool finishedReading;
    std::string result;
    std::vector<uint32_t> move_list;
    OpeningTablebase *m_opening_tablebase;

    bool read_metadata_line(std::string &line);
    void process_player_move(std::string player_move, bool white);
    void process_result(std::string resultstr);
    bool read_game_move_line(std::string &line);
};
