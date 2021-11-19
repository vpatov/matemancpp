#pragma once

#include "tablebase/opening_tablebase.hpp"
#include "threadpool/threadpool.hpp"
#include <unordered_map>
#include <sstream>
#include <iomanip>

class MasterTablebase
{
public:
    static const uint16_t TABLEBASE_SHARD_COUNT = 64;
    OpeningTablebase m_tablebases[TABLEBASE_SHARD_COUNT];
    std::mutex mutexes[TABLEBASE_SHARD_COUNT];
    z_hash_t m_root_hash;

    std::shared_ptr<std::unordered_map<MoveKey, MoveEdge>> operator[](const z_hash_t position_hash)
    {
        return m_tablebases[position_hash % TABLEBASE_SHARD_COUNT].m_tablebase[position_hash];
    }

    MasterTablebase()
    {
        m_root_hash = zobrist_hash(starting_position().get());
    }

    // reads the tablebases from disk
    MasterTablebase(fs::path source_directory_path)
    {
        m_root_hash = zobrist_hash(starting_position().get());
        int count = 0;
        for (const auto &entry : std::filesystem::directory_iterator(source_directory_path))
        {
            std::string filepath = entry.path().generic_string();
            size_t path_end = filepath.rfind('/');
            size_t extension_start = filepath.rfind(".tb");
            std::string filename = filepath.substr(path_end + 1, extension_start - path_end - 1);
            uint16_t bucket = std::stoi(filename);
            m_tablebases[bucket].read_from_file(filepath);
            count++;
        }
        std::cout << ColorCode::green << "Sucessfully read "
                  << ColorCode::yellow << count << ColorCode::green << " tablebases"
                  << ColorCode::end << std::endl;
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

    bool position_exists(z_hash_t position_hash)
    {
        uint16_t bucket = position_hash % TABLEBASE_SHARD_COUNT;
        return m_tablebases[bucket].position_exists(position_hash);
    }

    void update(z_hash_t insert_hash, z_hash_t dest_hash, MoveKey move_key, std::string pgn_move)
    {
        uint16_t bucket = insert_hash % TABLEBASE_SHARD_COUNT;
        std::unique_lock<std::mutex> lock(mutexes[bucket]);

        m_tablebases[bucket].update(insert_hash, dest_hash, move_key, pgn_move);
    }

    std::string pick_move_from_sample(z_hash_t position_hash)
    {
        uint16_t bucket = position_hash % TABLEBASE_SHARD_COUNT;
        std::cout << "modulo says bucket: " << bucket << std::endl;
        for (int i = 0; i < 64; i++)
        {
            auto res = m_tablebases[i].m_tablebase.find(position_hash);
            if (res != m_tablebases[i].m_tablebase.end())
            {
                std::cout << "found this position in bucket: " << i << std::endl;
            }
        }
        return m_tablebases[bucket].pick_move_from_sample(position_hash);
    }

    void walk_down_most_popular_path()
    {
        std::cout << ColorCode::purple << "root hash: " << m_root_hash << ColorCode::end << std::endl;
        uint16_t bucket = m_root_hash % TABLEBASE_SHARD_COUNT;
        auto root = m_tablebases[bucket].m_tablebase.find(m_root_hash);
        assert(root != m_tablebases[bucket].m_tablebase.end());

        std::queue<decltype(root)> to_visit;
        std::set<z_hash_t> visited;
        to_visit.push(root);

        int depth = 0;

        while (!to_visit.empty())
        {
            auto node = to_visit.front();

            if (visited.find(node->first) != visited.end())
            {
                break;
            }
            else
            {
                visited.insert(node->first);
            }

            to_visit.pop();
            depth++;

            auto key_move_pair = *(
                std::max_element(
                    node->second->begin(), node->second->end(), &compare_key_move_pair));

            MoveKey most_popular_move_key = key_move_pair.first;
            MoveEdge *most_popular_move = &key_move_pair.second;

            std::cout << "Depth: " << depth << std::endl;
            std::cout << most_popular_move_key << std::endl;
            std::cout << *most_popular_move << std::endl;

            int next_bucket = most_popular_move->m_dest_hash % TABLEBASE_SHARD_COUNT;
            auto next_node = m_tablebases[next_bucket].m_tablebase.find(most_popular_move->m_dest_hash);
            if (next_node != m_tablebases[next_bucket].m_tablebase.end())
            {
                to_visit.push(next_node);
            }
        }
    }

    // void walk_through_first_bucket()
    // {
    //     m_tablebases[0].walk_down_most_popular_path();
    // }

    // void walk_through_all_buckets()
    // {
    //     for (int i = 0; i < TABLEBASE_SHARD_COUNT; i++)
    //     {
    //         std::cout << "Walking through bucket " << i << "..." << std::endl;
    //         m_tablebases[i].walk_down_most_popular_path();
    //     }
    // }

    int
    total_size()
    {
        int s = 0;
        for (int i = 0; i < TABLEBASE_SHARD_COUNT; i++)
        {
            s += m_tablebases[i].m_tablebase.size();
        }
        return s;
    }
};

extern MasterTablebase engine_opening_tablebase;
