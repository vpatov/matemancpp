#pragma once

#include "tablebase.hpp"
#include "threadpool.hpp"
#include <sstream>
#include <iomanip>

struct MasterTablebase
{
    static const uint16_t TABLEBASE_SHARD_COUNT = 64;

    // pointers ? or just the opening tablebases?
    OpeningTablebase tablebases[TABLEBASE_SHARD_COUNT];

    std::mutex mutexes[TABLEBASE_SHARD_COUNT];

    // TODO look at dissasemble of this method and see if modulo is optimized out.
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
};

extern MasterTablebase masterTablebase;
