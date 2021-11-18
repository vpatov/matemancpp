#pragma once

#include "tablebase/opening_tablebase.hpp"
#include "threadpool/threadpool.hpp"
#include <unordered_map>
#include <sstream>
#include <iomanip>

class MasterTablebase
{
    static const uint16_t TABLEBASE_SHARD_COUNT = 64;
    OpeningTablebase m_tablebases[TABLEBASE_SHARD_COUNT];
    std::mutex mutexes[TABLEBASE_SHARD_COUNT];

    std::shared_ptr<std::unordered_map<MoveKey, MoveEdge>> operator[](const z_hash_t position_hash)
    {
        return m_tablebases[position_hash % TABLEBASE_SHARD_COUNT].m_tablebase[position_hash];
    }

public:
    MasterTablebase() {}
    MasterTablebase(fs::path source_file_path)
    {
        // deserialize
    }

    static uint16_t get_shard_count()
    {
        return TABLEBASE_SHARD_COUNT;
    }

    void serialize_all(fs::path destination_directory_path)
    {
        fs::create_directories(destination_directory_path);
        std::function<void(std::string &)> serialize_fn =
            std::bind(&MasterTablebase::serialize_tablebase, this, std::placeholders::_1);

        ThreadPool thread_pool = ThreadPool();
        for (int bucket = 0; bucket < MasterTablebase::get_shard_count(); bucket++)
        {
            std::stringstream file_suffix;
            file_suffix << std::setw(10) << std::setfill('0') << bucket;

            Task task = Task(&serialize_fn, (destination_directory_path / file_suffix.str()));
            thread_pool.add_task(task);
        }
        thread_pool.join_pool();
    }

    // gets the bucket number from the filepath, then reads that bucket from the master tablebases,
    // and serializes it
    void serialize_tablebase(std::string file_path_prefix)
    {
        uint16_t bucket = std::stoi(file_path_prefix.substr(file_path_prefix.rfind("/") + 1));
        m_tablebases[bucket].serialize_tablebase(file_path_prefix + ".tb");
    }

    void update(z_hash_t insert_hash, z_hash_t dest_hash, MoveKey move_key, std::string pgn_move)
    {
        uint16_t bucket = insert_hash % TABLEBASE_SHARD_COUNT;
        std::unique_lock<std::mutex> lock(mutexes[bucket]);

        m_tablebases[bucket].update(insert_hash, dest_hash, move_key, pgn_move);
    }

    int total_size()
    {
        int s = 0;
        for (int i = 0; i < TABLEBASE_SHARD_COUNT; i++)
        {
            s += m_tablebases[i].m_tablebase.size();
        }
        return s;
    }
};

extern MasterTablebase masterTablebase;
