#pragma once
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

const fs::path project_root_dir = (PROJECT_ROOT_DIR); // PROJECT_ROOT_DIR is defined in CMakeLists.txt
const fs::path dev_data_dir = project_root_dir / "dev_data";
const fs::path tablebase_data_dir = dev_data_dir / "tablebase";
const fs::path individual_tablebase_data_dir = tablebase_data_dir / "individual_tablebases";
const fs::path master_tablebase_data_dir = tablebase_data_dir / "master_tablebase";

const bool use_most_recent_tablebase = true;

const auto latest_tablebase_timestamp = std::to_string(std::chrono::seconds(std::time(NULL)).count());
const std::string master_tablebase_filepath =
    "/Users/vas/repos/matemancpp/dev_data/tablebase/master_tablebase";
const std::string individual_tablebases_filepath =
    "/Users/vas/repos/matemancpp/dev_data/tablebase/individual_tablebases";

// const std::string timestamp_of_attempt_to_use = "1630933103";
const std::string timestamp_of_attempt_to_use = "";

const std::string latest_individual_tablebases_filepath =
    individual_tablebases_filepath +
    '/' + (timestamp_of_attempt_to_use.empty() ? latest_tablebase_timestamp : timestamp_of_attempt_to_use);

const std::string latest_master_tablebase_filepath =
    master_tablebase_filepath +
    '/' + (timestamp_of_attempt_to_use.empty() ? latest_tablebase_timestamp : timestamp_of_attempt_to_use);

/**
 *  Binary format for tablebase serialization (8 bits == byte):
 * 
 *  {
 *      8 bytes (64 bits): root hashkey
 *  }
 *  {
 *      4 bytes (32 bits): # of key/value pairs (N)
 *  }
 *  Key/Value Pair:
 *  {
 *      Key:
 *      {
 *          8 bytes (64 bits) -> z_hash_t: key
 *      }
 *      Value: 
 *      {
 *          4 bytes (32 bits) -> uint32_t: length of MoveEdge map (M)
 * 
 *          MoveKey: {
 *              4 bytes (32 bits) -> uint32_t: move key
 *          }
 *          MoveEdge: {
 *              8 bytes (64 bits) -> z_hash_t: destination hash 
 *              8 bytes (64 bits) -> char[]:   pgn move
 *              4 bytes (32 bits) -> uint32_t: times played
 *          } x M
 *      }
 *  } x N
 * */

class TablebasePersistor
{
    // void serialize_tablebase(std::string file_path);

    // void serialize_tablebase(OpeningTablebase *tablebase, std::string file_path)
    // {
    //     std::fstream stream(file_path, std::ios::out | std::ios::binary);

    //     // 8 bytes (464bits) -> z_hash_t: hash of starting position
    //     write(&stream, &tablebase->m_root_hash, sizeof(z_hash_t));

    //     // 4 bytes (4 bits) -> uint32_t: number of keys in tablebase
    //     uint32_t tablebase_size = tablebase->m_tablebase.size();
    //     write(&stream, &tablebase_size, sizeof(uint32_t));

    //     for (auto node = m_tablebase.begin(); node != m_tablebase.end(); node++)
    //     {
    //         z_hash_t key_hash = node->first;
    //         auto move_map = node->second;

    //         // 8 bytes (64 bits) -> z_hash_t: key
    //         write(&stream, &key_hash, sizeof(z_hash_t));

    //         // // 4 bytes (32 bits) -> uint32_t: length of MoveEdge map (M)
    //         uint32_t move_map_size = move_map->size();
    //         write(&stream, &move_map_size, sizeof(uint32_t));

    //         // MoveEdge serialization (4, 8, 8, 4)
    //         for (auto it = move_map->begin(); it != move_map->end(); it++)
    //         {
    //             MoveKey t_move_key = it->first;
    //             write(&stream, &(t_move_key), sizeof(MoveKey));
    //             write(&stream, &(it->second.m_dest_hash), sizeof(MoveEdge::m_dest_hash));
    //             write(&stream, &(it->second.m_pgn_move), sizeof(MoveEdge::m_pgn_move));
    //             write(&stream, &(it->second.m_times_played), sizeof(MoveEdge::m_times_played));
    //         }
    //     }

    //     stream.close();
    // }
};
