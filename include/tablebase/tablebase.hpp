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
#include "tablebase/movekey.hpp"

struct MoveEdge
{
    z_hash_t m_dest_hash;
    uint32_t m_times_played;

    char m_pgn_move[8];

    friend std::ostream &operator<<(std::ostream &os, MoveEdge &move_edge);

    MoveEdge(z_hash_t dest_hash, std::string pgn_move)
    {
        m_dest_hash = dest_hash;
        assert(pgn_move.size() < 8);
        strncpy(m_pgn_move, pgn_move.c_str(), 8);

        m_times_played = 1;
    };

    MoveEdge(z_hash_t dest_hash, std::string pgn_move, uint32_t times_played)
    {
        m_dest_hash = dest_hash;
        assert(pgn_move.size() < 8);
        strncpy(m_pgn_move, pgn_move.c_str(), 8);

        m_times_played = times_played;
    };

    MoveEdge() {}

    MoveEdge(const MoveEdge &other)
    {
        m_dest_hash = other.m_dest_hash;
        m_times_played = other.m_times_played;
        strncpy(m_pgn_move, other.m_pgn_move, 8);
    }

    MoveEdge(MoveEdge &&other)
    {
        m_dest_hash = other.m_dest_hash;
        m_times_played = other.m_times_played;
        strncpy(m_pgn_move, other.m_pgn_move, 8);
    }
};

typedef std::__1::unordered_map<z_hash_t, std::__1::shared_ptr<std::__1::unordered_map<MoveKey, MoveEdge>>>::iterator tablebase_iter;

using MovesPlayed = std::unordered_map<MoveKey, MoveEdge>;
using PositionToMovesPlayedMap = std::unordered_map<z_hash_t, std::shared_ptr<MovesPlayed>>;

uint32_t generate_move_key(uint8_t src_square, uint8_t dest_square, uint8_t promotion_piece);
std::string generate_long_algebraic_notation(MoveKey move_key);
bool compare_key_move_pair(std::pair<MoveKey, MoveEdge> p1, std::pair<MoveKey, MoveEdge> p2);
std::ostream &operator<<(std::ostream &os, MoveEdge &move_edge);

class Tablebase
{
public:
    static const uint16_t TABLEBASE_SHARD_COUNT = 64;

    // LASTLEFTOFF
    // remove OepningTablebase m_tablebases line and fix compiler errors.
    // then probably just get rid of the opening tablebase class.
    // then put the serialization logic into a separate file.
    // also fix function bind that doesnt compile
    // OpeningTablebase m_tablebases[TABLEBASE_SHARD_COUNT];

    PositionToMovesPlayedMap shards[TABLEBASE_SHARD_COUNT];
    std::mutex mutexes[TABLEBASE_SHARD_COUNT];
    z_hash_t m_root_hash;

    std::shared_ptr<MovesPlayed> operator[](const z_hash_t position_hash)
    {
        return shards[position_hash % TABLEBASE_SHARD_COUNT][position_hash];
    }

    void read_from_file(std::string file_path, int shard)
    {
        std::streampos size;
        char *data;
        int index = 0;
        std::ifstream infile(file_path, std::ios::binary | std::ios::ate);

        if (infile.is_open())
        {
            size = infile.tellg();

            data = new char[size];
            infile.seekg(0, std::ios::beg);
            infile.read(data, size);
            infile.close();
        }
        else
        {
            std::cerr << ColorCode::red << "Could not open file: " << file_path << ColorCode::end << std::endl;
            assert(false);
        }

        // m_root_hash = *((z_hash_t *)(data + index));
        index += sizeof(z_hash_t);

        uint32_t tablebase_size = *((uint32_t *)(data + index));
        index += sizeof(uint32_t);

        for (int n = 0; n < tablebase_size; n++)
        {
            z_hash_t key_hash = *((z_hash_t *)(data + index));
            index += sizeof(z_hash_t);

            uint32_t move_map_size = *((uint32_t *)(data + index));
            index += sizeof(uint32_t);

            std::shared_ptr<std::unordered_map<MoveKey, MoveEdge>> move_map =
                std::make_shared<std::unordered_map<MoveKey, MoveEdge>>();

            for (int m = 0; m < move_map_size; m++)
            {

                MoveKey move_key = *((uint32_t *)(data + index));
                assert((move_key >> 24) == 0);
                index += sizeof(MoveKey);

                z_hash_t dest_hash = *((z_hash_t *)(data + index));
                index += sizeof(MoveEdge::m_dest_hash);

                char pgn_move[8];
                strncpy(pgn_move, data + index, sizeof(MoveEdge::m_pgn_move));
                index += sizeof(MoveEdge::m_pgn_move);

                uint32_t times_played = *((uint32_t *)(data + index));
                index += sizeof(MoveEdge::m_times_played);

                MoveEdge moveEdge(dest_hash, pgn_move, times_played);

                move_map->insert(std::pair(move_key, moveEdge));
            }

            shards[shard][key_hash] = move_map;
        }
    }

    Tablebase()
    {
        m_root_hash = zobrist_hash(starting_position().get());
    }

    template <typename T>
    static void write(std::fstream *stream, T *data, size_t size)
    {
        stream->write(reinterpret_cast<char *>(data), size);
    }

    // reads the tablebases from disk
    Tablebase(fs::path source_directory_path)
    {
        m_root_hash = zobrist_hash(starting_position().get());
        int count = 0;
        for (const auto &entry : std::filesystem::directory_iterator(source_directory_path))
        {
            std::string filepath = entry.path().generic_string();
            size_t path_end = filepath.rfind('/');
            size_t extension_start = filepath.rfind(".tb");
            std::string filename = filepath.substr(path_end + 1, extension_start - path_end - 1);
            uint16_t shard = std::stoi(filename);
            read_from_file(filepath, shard);
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

    void serialize_tablebase(std::string file_path, int shard)
    {
        std::fstream stream(file_path, std::ios::out | std::ios::binary);

        if (!stream.is_open())
        {
            std::cerr
                << ColorCode::red << "Cannot open filestream to path: " << ColorCode::end << std::endl
                << file_path << std::endl;
            return;
        }

        // 8 bytes (464bits) -> z_hash_t: hash of starting position
        write(&stream, &m_root_hash, sizeof(z_hash_t));

        // 4 bytes (4 bits) -> uint32_t: number of keys in tablebase
        auto position_moves_map = shards[shard];
        uint32_t tablebase_size = position_moves_map.size();
        write(&stream, &tablebase_size, sizeof(uint32_t));

        for (auto node = position_moves_map.begin(); node != position_moves_map.end(); node++)
        {
            z_hash_t key_hash = node->first;
            auto move_map = node->second;

            // 8 bytes (64 bits) -> z_hash_t: key
            write(&stream, &key_hash, sizeof(z_hash_t));

            // // 4 bytes (32 bits) -> uint32_t: length of MoveEdge map (M)
            uint32_t move_map_size = move_map->size();
            write(&stream, &move_map_size, sizeof(uint32_t));

            // MoveEdge serialization (4, 8, 8, 4)
            for (auto it = move_map->begin(); it != move_map->end(); it++)
            {
                MoveKey t_move_key = it->first;
                write(&stream, &(t_move_key), sizeof(MoveKey));
                write(&stream, &(it->second.m_dest_hash), sizeof(MoveEdge::m_dest_hash));
                write(&stream, &(it->second.m_pgn_move), sizeof(MoveEdge::m_pgn_move));
                write(&stream, &(it->second.m_times_played), sizeof(MoveEdge::m_times_played));
            }
        }

        stream.close();
    }

    void serialize_all(fs::path destination_directory_path)
    {
        fs::create_directories(destination_directory_path);

        ThreadPool thread_pool = ThreadPool();
        for (int shard = 0; shard < Tablebase::get_shard_count(); shard++)
        {
            std::stringstream file_suffix;
            file_suffix << std::setw(10) << std::setfill('0') << shard;

            // std::function<void(std::string &)> serialize_fn =
            //     std::bind(&Tablebase::serialize_tablebase, this, std::placeholders::_1, shard);

            // Task task = Task(&serialize_fn, (destination_directory_path / file_suffix.str()));
            // thread_pool.add_task(task);
        }
        thread_pool.join_pool();
    }

    // gets the bucket number from the filepath, then reads that bucket from the master tablebases,
    // and serializes it
    void serialize_tablebase(std::string file_path_prefix)
    {
        uint16_t shard = std::stoi(file_path_prefix.substr(file_path_prefix.rfind("/") + 1));
        serialize_tablebase(file_path_prefix + ".tb", shard);
    }

    // TODO test this
    bool position_exists(z_hash_t position_hash)
    {
        return (*this)[position_hash] != NULL;
    }

    void update(z_hash_t insert_hash, z_hash_t dest_hash, MoveKey move_key, std::string pgn_move)
    {
        uint16_t shard = insert_hash % TABLEBASE_SHARD_COUNT;
        std::unique_lock<std::mutex> lock(mutexes[shard]);

        auto position_moveset_map = shards[shard];

        MoveEdge moveEdge(dest_hash, pgn_move);

        // find the node corresponding to the zobrist hash for the position we are inserting at.
        auto node = position_moveset_map.find(insert_hash);

        if (node == position_moveset_map.end())
        {
            insert_new_move_map(insert_hash, move_key, moveEdge);
        }
        else
        {
            increment_times_played_or_insert_move(node, move_key, moveEdge);
        }
    }

    void insert_new_move_map(z_hash_t insert_hash, MoveKey moveKey, MoveEdge moveEdge)
    {
        std::shared_ptr<std::unordered_map<MoveKey, MoveEdge>> move_map =
            std::make_shared<std::unordered_map<MoveKey, MoveEdge>>();
        move_map->insert(std::pair(moveKey, moveEdge));
        uint8_t shard = insert_hash % TABLEBASE_SHARD_COUNT;
        shards[shard][insert_hash] = move_map;
    }

    void increment_times_played_or_insert_move(
        tablebase_iter node, MoveKey moveKey, MoveEdge moveEdge)
    {
        // if the move exists already, just increment the times its been played
        // TODO name these variables better
        auto it = node->second->begin();
        for (; it != node->second->end(); it++)
        {
            if (it->first == moveKey)
            {
                assert(it->second.m_dest_hash == moveEdge.m_dest_hash);
                it->second.m_times_played++;
                break;
            }
        }

        // if we couldn't find the move in the move list, it's a new move
        if (it == node->second->end())
        {
            node->second->insert(std::pair(moveKey, moveEdge));
        }
    }

    std::string pick_move_from_sample(z_hash_t position_hash)
    {
        uint16_t shard = position_hash % TABLEBASE_SHARD_COUNT;

        auto position_moveset_map = shards[shard];
        auto node = position_moveset_map.find(position_hash);

        if (node == position_moveset_map.end())
        {
            return "";
        }

        uint32_t sum_times_played = 0;
        for (auto it = node->second->begin(); it != node->second->end(); it++)
        {
            auto move = *it;
            sum_times_played += move.second.m_times_played;
        }

        // random from (0,to sum_times_played)
        // iterate through moves, and check to see if random var is less than the running sum
        // of times played. If a move was played X% of the time, it will be picked X% of the time.
        uint32_t random_var = random_bitstring() % sum_times_played; // is this evenly distributed?
        uint32_t addend = 0;
        for (auto it = node->second->begin(); it != node->second->end(); it++)
        {
            auto move = *it;
            addend += move.second.m_times_played;
            if (random_var < addend)
            {
                auto move_key = move.first;
                return generate_long_algebraic_notation(move_key);
            }
        }

        // should not reach this point, because our random number should be
        // less than the sum of the times_played in this node, and our condition should trigger
        assert(false);
        return "";
    }

    void walk_down_most_popular_path()
    {
        std::cout << ColorCode::purple << "root hash: " << m_root_hash << ColorCode::end << std::endl;
        uint16_t shard = m_root_hash % TABLEBASE_SHARD_COUNT;
        auto root = shards[shard].find(m_root_hash);
        assert(root != shards[shard].end());

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

            uint8_t next_shard = most_popular_move->m_dest_hash % TABLEBASE_SHARD_COUNT;
            auto next_node = shards[next_shard].find(most_popular_move->m_dest_hash);
            if (next_node != shards[next_shard].end())
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
        for (int shard = 0; shard < TABLEBASE_SHARD_COUNT; shard++)
        {
            s += shards[shard].size();
        }
        return s;
    }
};

extern Tablebase engine_opening_tablebase;
