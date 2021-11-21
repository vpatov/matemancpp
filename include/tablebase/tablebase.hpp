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
#include "tablebase/move.hpp"

typedef std::__1::unordered_map<z_hash_t, std::__1::shared_ptr<std::__1::unordered_map<MoveKey, MoveEdge>>>::iterator tablebase_iter;

using MovesPlayed = std::unordered_map<MoveKey, MoveEdge>;
using PositionToMovesPlayedMap = std::unordered_map<z_hash_t, std::shared_ptr<MovesPlayed>>;

std::string generate_long_algebraic_notation(MoveKey move_key);

class Tablebase
{
public:
    static const uint16_t TABLEBASE_SHARD_COUNT = 64;

    // LASTLEFTOFF
    // separate this file into .hpp and .cpp
    // order members and functions properly
    // clean up
    // remove dead code
    // public vs private

    void read_from_file(std::string file_path, int shard);
    void read_from_directory(fs::path source_directory_path);
    void serialize_tablebase(std::string file_path, int shard);
    void serialize_all(fs::path destination_directory_path);

    PositionToMovesPlayedMap shards[TABLEBASE_SHARD_COUNT];
    std::mutex mutexes[TABLEBASE_SHARD_COUNT];
    z_hash_t m_root_hash;

    std::shared_ptr<MovesPlayed> operator[](const z_hash_t position_hash)
    {
        return shards[position_hash % TABLEBASE_SHARD_COUNT][position_hash];
    }

    Tablebase()
    {
        m_root_hash = zobrist_hash(starting_position().get());
    }

    Tablebase(fs::path source_directory_path)
    {
        m_root_hash = zobrist_hash(starting_position().get());
        read_from_directory(source_directory_path);
    }

    static uint16_t get_shard_count()
    {
        return TABLEBASE_SHARD_COUNT;
    }

    bool position_exists(z_hash_t position_hash);
    void update(z_hash_t insert_hash, z_hash_t dest_hash, MoveKey move_key, std::string pgn_move);
    void insert_new_move_map(z_hash_t insert_hash, MoveKey moveKey, MoveEdge moveEdge);
    void increment_times_played_or_insert_move(tablebase_iter node, MoveKey moveKey, MoveEdge moveEdge);
    std::string pick_move_from_sample(z_hash_t position_hash);
    void walk_down_most_popular_path();
    int total_size();
};
