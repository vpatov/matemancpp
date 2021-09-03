#pragma once

#include "tablebase.hpp"
#include <iostream>
#include <fstream>

struct metadata_entry
{
    std::string key;
    std::string value;
};

namespace ColorCode
{
    static const std::string white = "\u001b[37m";
    static const std::string teal = "\u001b[36m";
    static const std::string purple = "\u001b[35m";
    static const std::string blue = "\u001b[34m";
    static const std::string yellow = "\u001b[33m";
    static const std::string green = "\u001b[32m";
    static const std::string red = "\u001b[31m";
    static const std::string end = "\u001b[0m";
};

struct PgnGame
{

    std::vector<metadata_entry> m_metadata;
    Position m_position;
    int m_whiteElo;
    int m_blackElo;
    bool m_eloOverThreshold;
    bool m_finishedReading;
    std::string m_result;
    std::string m_event;
    std::string m_white_player_name;
    std::string m_black_player_name;
    std::vector<uint32_t> m_move_list;
    OpeningTablebase *m_opening_tablebase;

    bool read_metadata_line(std::string &line);
    void process_player_move(std::string player_move, bool white);
    void process_result(std::string resultstr);
    bool read_game_move_line(std::string &line);
    void populateMetadata();
    void printGameSummary();

    static void printGameSummaryHeader()
    {

        using namespace std;
        cout
            << ColorCode::teal
            << left << setw(32) << "White"
            << left << setw(32) << "Black"
            << left << setw(30) << "Event"
            << left << setw(14) << "Result"
            << left << setw(10) << "# Moves"
            << left << setw(10) << "White Elo"
            << left << setw(10) << "Black Elo"
            << ColorCode::end
            << std::endl;
    }
};
