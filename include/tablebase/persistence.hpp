#pragma once
#include <string>
#include <filesystem>
#include <set>

namespace fs = std::filesystem;

struct TablebasePersistenceConfig
{
    const fs::path tablebase_data_dir = dev_data_dir / "tablebase";
    const fs::path master_tablebase_data_dir = tablebase_data_dir / "master_tablebase";

    std::string m_tablebase_name;
    fs::path m_current_master_tablebase_directory;

    TablebasePersistenceConfig(std::string tablebase_name)
    {
        m_tablebase_name = tablebase_name;
        m_current_master_tablebase_directory = master_tablebase_data_dir / tablebase_name;

        std::filesystem::create_directories(m_current_master_tablebase_directory);
    }
};

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