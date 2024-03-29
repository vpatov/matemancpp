#pragma once

#include <iostream>
#include <fstream>
#include "representation/position.hpp"
#include "tablebase/tablebase.hpp"

struct metadata_entry
{
    std::string key;
    std::string value;
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

    bool read_metadata_line(std::string &line);
    void process_player_move(std::string player_move, bool whites_turn, Tablebase *masterTablebase);
    void process_result(std::string resultstr);
    bool read_game_move_line(std::string &line, Tablebase *masterTablebase, int max_plies);
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
