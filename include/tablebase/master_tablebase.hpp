#pragma once

#include "tablebase/opening_tablebase.hpp"
#include <unordered_map>
#include <sstream>
#include <iomanip>

struct MasterTablebase
{
    static const uint16_t TABLEBASE_SHARD_COUNT = 64;
    OpeningTablebase tablebases[TABLEBASE_SHARD_COUNT];
    std::mutex mutexes[TABLEBASE_SHARD_COUNT];

    std::shared_ptr<std::unordered_map<MoveKey, MoveEdge>> operator[](const z_hash_t position_hash)
    {
        return tablebases[position_hash % TABLEBASE_SHARD_COUNT].m_tablebase[position_hash];
    }

    void update(z_hash_t insert_hash, z_hash_t dest_hash, MoveKey move_key, std::string pgn_move)
    {
        uint16_t bucket = insert_hash % TABLEBASE_SHARD_COUNT;
        std::unique_lock<std::mutex> lock(mutexes[bucket]);

        tablebases[bucket].update(insert_hash, dest_hash, move_key, pgn_move);
    }

    int total_size()
    {
        int s = 0;
        for (int i = 0; i < TABLEBASE_SHARD_COUNT; i++)
        {
            s += tablebases[i].m_tablebase.size();
        }
        return s;
    }
};

extern MasterTablebase masterTablebase;
