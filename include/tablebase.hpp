#pragma once

#include "util.hpp"
#include "position.hpp"
#include "squares.hpp"
#include "zobrist.hpp"
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <queue>
#include <fstream>
#include <iostream>
#include <type_traits>
#include <cstring>
#include <thread>

// TODO SERIALIZATION OF OPENING TABLEBASE
// TODO MERGING OF OPENING TABLEBASE FROM DIFFERENT THREADS
// LASTLEFTOFF
/*
  How to handle multithreaded logging
  Options:
  1) Each thread logs to their own file. 
  Pros: 
    - Simplest, least amount of code probably.
    - No locks required
  Cons:
    - aggregation requires file reading which is a bit slower (probably not slower in a meaningful way)
  2) Each thread logs to their own spot in memory, separate thread can provide summary in stdout and write to one file.
  Pros: 
  - Easier to provide summary
  - Seems efficient
  Cons:
  - Have to coordinate memory for threads

*/

struct MoveEdge
{
  z_hash_t m_dest_hash;
  uint32_t m_times_played;
  // move key is bit-wise concatenation of
  // 0x00 + start_square + end_square + promotion_piece
  uint32_t m_move_key;
  char m_pgn_move[8];

  friend std::ostream &operator<<(std::ostream &os, MoveEdge &move_edge);

  MoveEdge(z_hash_t dest_hash, uint32_t move_key, std::string pgn_move)
  {
    m_dest_hash = dest_hash;
    m_move_key = move_key;
    assert(pgn_move.size() < 8);
    strncpy(m_pgn_move, pgn_move.c_str(), 8);

    m_times_played = 1;
  };

  MoveEdge() {}

  // MoveEdge(z_hash_t dest_hash, char pgn_move[8], uint32_t times_played, uint32_t move_key ){

  // }
};
bool compare_move_edge(MoveEdge move_edge1, MoveEdge move_edge2);

typedef std::__1::unordered_map<z_hash_t, std::__1::shared_ptr<std::__1::vector<MoveEdge>>>::iterator tablebase_iter;

struct OpeningTablebase
{
  // TODO replace with unordered_muiltimap
  std::unordered_map<z_hash_t, std::shared_ptr<std::vector<MoveEdge>>> m_tablebase;
  z_hash_t m_root_hash;

  void update(z_hash_t insert_hash, z_hash_t dest_hash, uint32_t move_key, std::string pgn_move);

  void insert_new_move_edge_list(z_hash_t insert_hash, MoveEdge moveEdge);

  void increment_times_played_or_insert_move(tablebase_iter node, MoveEdge moveEdge);

  void walk_down_most_popular_path();

  template <typename T>
  void write(std::fstream *stream, T *data, size_t size)
  {
    stream->write(reinterpret_cast<char *>(data), size);
  }

  /*
    Binary format for tablebase serialization (8 bits == byte):

    {
      8 bytes (64 bits): root hashkey
    }
    {
      4 bytes (32 bits): # of key/value pairs (N)
    }
    Key/Value Pair:
    {
      Key:
      {
        8 bytes (64 bits) -> z_hash_t: key
      }
      Value: 
      {
        4 bytes (32 bits) -> uint32_t: length of MoveEdge vector (M)
        MoveEdge: {
          8 bytes (64 bits) -> z_hash_t: destination hash 
          8 bytes (64 bits) -> char[]:   pgn move
          4 bytes (32 bits) -> uint32_t: times played
          4 bytes (32 bits) -> uint32_t: move key
        } x M
      }
    } x N
  */
  void serialize_tablebase(std::string file_path)
  {
    std::fstream stream(file_path, std::ios::out | std::ios::binary);

    // 8 bytes (464bits) -> z_hash_t: hash of starting position
    write(&stream, &m_root_hash, sizeof(z_hash_t));

    // 4 bytes (4 bits) -> uint32_t: number of keys in tablebase
    uint32_t tablebase_size = m_tablebase.size();
    write(&stream, &tablebase_size, sizeof(uint32_t));

    for (auto node = m_tablebase.begin(); node != m_tablebase.end(); node++)
    {
      z_hash_t key_hash = node->first;
      auto move_list = node->second;

      // 8 bytes (64 bits) -> z_hash_t: key
      write(&stream, &key_hash, sizeof(z_hash_t));

      // // 4 bytes (32 bits) -> uint32_t: length of MoveEdge vector (M)
      uint32_t move_list_size = move_list->size();
      write(&stream, &move_list_size, sizeof(uint32_t));

      // MoveEdge serialization (8, 8, 4, 4)
      for (auto it = move_list->begin(); it != move_list->end(); it++)
      {
        write(&stream, &(it->m_dest_hash), sizeof(z_hash_t));
        write(&stream, &(it->m_pgn_move), sizeof(MoveEdge::m_pgn_move));
        write(&stream, &(it->m_times_played), sizeof(uint32_t));
        write(&stream, &(it->m_move_key), sizeof(uint32_t));
      }
    }

    stream.close();
  }

  static OpeningTablebase deserialize_tablebase(std::string file_path)
  {
    std::streampos size;
    char *data;
    int index = 0;
    OpeningTablebase deserialized_tablebase;
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
      assert(false);
    }

    deserialized_tablebase.m_root_hash = *((z_hash_t *)(data + index));
    index += sizeof(z_hash_t);

    uint32_t tablebase_size = *((uint32_t *)(data + index));
    index += sizeof(uint32_t);

    for (int n = 0; n < tablebase_size; n++)
    {
      z_hash_t key_hash = *((z_hash_t *)(data + index));
      index += sizeof(z_hash_t);

      uint32_t move_list_size = *((uint32_t *)(data + index));
      index += sizeof(uint32_t);

      std::shared_ptr<std::vector<MoveEdge>> move_list = std::make_shared<std::vector<MoveEdge>>();

      for (int m = 0; m < move_list_size; m++)
      {

        MoveEdge moveEdge;

        moveEdge.m_dest_hash = *((z_hash_t *)(data + index));
        index += sizeof(MoveEdge::m_dest_hash);

        strncpy(moveEdge.m_pgn_move, data + index, sizeof(MoveEdge::m_pgn_move));
        index += sizeof(MoveEdge::m_pgn_move);

        moveEdge.m_times_played = *((uint32_t *)(data + index));
        index += sizeof(MoveEdge::m_times_played);

        moveEdge.m_move_key = *((uint32_t *)(data + index));
        index += sizeof(MoveEdge::m_move_key);

        move_list->push_back(moveEdge);
      }

      deserialized_tablebase.m_tablebase[key_hash] = move_list;
    }

    return deserialized_tablebase;
  }

  static std::vector<OpeningTablebase> read_tablebases(std::string tablebase_filepath)
  {
    std::vector<OpeningTablebase> openingTablebases;

    for (const auto &entry : std::filesystem::directory_iterator(tablebase_filepath))
    {
      std::string filepath_str = entry.path().generic_string();
      std::cout
          << ColorCode::purple << filepath_str.substr(filepath_str.find_last_of('/') + 1)
          << ColorCode::end << std::endl;
      openingTablebases.push_back(deserialize_tablebase(entry.path()));
    }
    return openingTablebases;
  }

  static std::shared_ptr<OpeningTablebase> merge_tablebases(
      std::unordered_multimap<std::thread::id, std::shared_ptr<OpeningTablebase>> *thread_tablebase_map)
  {
    std::shared_ptr<OpeningTablebase> merged_tablebase = std::make_shared<OpeningTablebase>();
    auto tb = &(merged_tablebase->m_tablebase);

    int count = 0;
    std::cout << ColorCode::teal << "Number of thread/tablebase pairs: "
              << thread_tablebase_map->size() << ColorCode::end << std::endl;

    std::cout
        << std::left << std::setw(20) << "Thread"
        << std::left << std::setw(20) << "Tablebase Address"
        << std::left << std::setw(20) << "# Positions"
        << std::endl;

    for (auto thread_tb_pair : *thread_tablebase_map)
    {
      auto tablebase = thread_tb_pair.second;
      std::cout
          << ColorCode::blue << std::left << std::setw(20) << thread_tb_pair.first
          << ColorCode::purple << std::left << std::setw(20) << thread_tb_pair.second
          << ColorCode::teal << std::left << std::setw(20) << tablebase->m_tablebase.size()
          << ColorCode::end << std::endl;

      if (tb->empty())
      {
        merged_tablebase->m_root_hash = tablebase->m_root_hash;
      }

      for (auto pair : tablebase->m_tablebase)
      {
        auto position_hash = pair.first;
        auto move_list = pair.second;

        auto res = tb->find(position_hash);

        if (res != tb->end())
        {
          auto vec = (*tb)[position_hash];
          vec->insert(vec->end(), move_list->begin(), move_list->end());
        }
        // position hasnt been seen yet, take the current move list
        else
        {
          (*tb)[position_hash] = std::move(move_list);
        }
      }
    }

    return merged_tablebase;
  }

  static std::shared_ptr<OpeningTablebase> merge_tablebases(
      std::vector<OpeningTablebase> *tablebase_vector)
  {
    std::shared_ptr<OpeningTablebase> merged_tablebase = std::make_shared<OpeningTablebase>();
    auto tb = &(merged_tablebase->m_tablebase);

    int count = 0;
    std::cout << ColorCode::teal << "Number of tablebases: "
              << tablebase_vector->size() << ColorCode::end << std::endl;

    std::cout
        << std::left << std::setw(20) << "Tablebase Address"
        << std::left << std::setw(20) << "# Positions"
        << std::endl;

    for (auto tablebase : *tablebase_vector)
    {
      std::cout
          << ColorCode::purple << std::left << std::setw(20) << &tablebase
          << ColorCode::teal << std::left << std::setw(20) << tablebase.m_tablebase.size()
          << ColorCode::end << std::endl;

      if (tb->empty())
      {
        merged_tablebase->m_root_hash = tablebase.m_root_hash;
      }

      for (auto pair : tablebase.m_tablebase)
      {
        auto position_hash = pair.first;
        auto move_list = pair.second;

        auto res = tb->find(position_hash);

        if (res != tb->end())
        {
          auto vec = (*tb)[position_hash];
          vec->insert(vec->end(), move_list->begin(), move_list->end());
        }
        // position hasnt been seen yet, take the current move list
        else
        {
          (*tb)[position_hash] = std::move(move_list);
        }
      }
    }

    return merged_tablebase;
  }
};
