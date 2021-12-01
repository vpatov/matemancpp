#pragma once

#include "threadpool/threadpool.hpp"
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <set>
#include "tablebase/zobrist.hpp"
#include "util.hpp"
#include "representation/move.hpp"
#include "tablebase/move_edge.hpp"

class Tablebase;

using TablebaseIter = std::__1::unordered_map<z_hash_t, std::__1::shared_ptr<std::__1::unordered_map<MoveKey, MoveEdge>>>::iterator;
using MovesPlayed = std::unordered_map<MoveKey, MoveEdge>;
using PositionToMovesPlayedMap = std::unordered_map<z_hash_t, std::shared_ptr<MovesPlayed>>;

class Tablebase
{
    static const uint16_t TABLEBASE_SHARD_COUNT = 64;
    PositionToMovesPlayedMap shards[TABLEBASE_SHARD_COUNT];
    std::mutex mutexes[TABLEBASE_SHARD_COUNT];
    z_hash_t m_root_hash;

public:
    void read_from_file(std::string file_path, int shard);
    void read_from_directory(fs::path source_directory_path);
    void serialize_tablebase(std::string file_path, int shard);
    void serialize_all(fs::path destination_directory_path);
    void test_fn(std::string file_path, int shard);

    Tablebase()
    {
        m_root_hash = zobrist_hash(starting_position().get());
    }

    Tablebase(fs::path source_directory_path)
    {
        m_root_hash = zobrist_hash(starting_position().get());
        read_from_directory(source_directory_path);
    }

    std::shared_ptr<MovesPlayed> operator[](const z_hash_t position_hash)
    {
        return shards[position_hash % TABLEBASE_SHARD_COUNT][position_hash];
    }

    static uint16_t get_shard_count()
    {
        return TABLEBASE_SHARD_COUNT;
    }

    bool operator==(const Tablebase &rhs) const
    {
        if (m_root_hash != rhs.m_root_hash)
            return false;

        for (size_t shard = 0; shard < TABLEBASE_SHARD_COUNT; shard++)
        {

            if (shards[shard].size() != rhs.shards[shard].size())
                return false;

            for (auto it = shards[shard].begin(); it != shards[shard].end(); it++)
            {
                auto hash = it->first;
                auto moves_played_ptr = it->second;
                auto rhs_moves_played_ptr = rhs.shards[shard].at(hash);

                if (moves_played_ptr->size() != rhs_moves_played_ptr->size())
                    return false;

                for (auto it2 = moves_played_ptr->begin(); it2 != moves_played_ptr->end(); it2++)
                {
                    auto res = rhs_moves_played_ptr->find(it2->first);
                    if (res == rhs_moves_played_ptr->end())
                    {
                        return false;
                    }
                    if (it2->second != res->second)
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    bool position_exists(z_hash_t position_hash);
    void update(z_hash_t insert_hash, z_hash_t dest_hash, MoveKey move_key, std::string pgn_move);
    void insert_new_move_map(z_hash_t insert_hash, MoveKey moveKey, MoveEdge moveEdge);
    void increment_times_played_or_insert_move(TablebaseIter node, MoveKey moveKey, MoveEdge moveEdge);
    MoveKey pick_move_from_sample(z_hash_t position_hash);
    void walk_down_most_popular_path();
    void list_all_moves_for_position(z_hash_t position_hash);

    int total_size();
};
