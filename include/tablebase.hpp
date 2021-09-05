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

// LASTLEFTOFF
// TODO move functionality into tablebase.cpp
// TODO add tests, assertions, and printouts
//  to ensure that serialization and merging methods are correct
// TODO figure out why tablebases are so much bigger than the pgn files they come from, which is
// counterintuitive

// move key is bit-wise concatenation of
// 0x00 + start_square + end_square + promotion_piece
typedef uint32_t MoveKey;

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

bool compare_move_edge(MoveEdge move_edge1, MoveEdge move_edge2);

typedef std::__1::unordered_map<z_hash_t, std::__1::shared_ptr<std::__1::unordered_map<MoveKey, MoveEdge>>>::iterator tablebase_iter;

struct OpeningTablebase
{
  std::unordered_map<z_hash_t, std::shared_ptr<std::unordered_map<MoveKey, MoveEdge>>> m_tablebase;
  z_hash_t m_root_hash;

  void update(z_hash_t insert_hash, z_hash_t dest_hash, MoveKey move_key, std::string pgn_move);

  void insert_new_move_map(z_hash_t insert_hash, MoveKey moveKey, MoveEdge moveEdge);

  void increment_times_played_or_insert_move(tablebase_iter node, MoveKey moveKey, MoveEdge moveEdge);

  void walk_down_most_popular_path();
  void walk_down_most_popular_path(std::string file_path);

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
        4 bytes (32 bits) -> uint32_t: length of MoveEdge map (M)

        MoveKey: {
          4 bytes (32 bits) -> uint32_t: move key
        }
        MoveEdge: {
          8 bytes (64 bits) -> z_hash_t: destination hash 
          8 bytes (64 bits) -> char[]:   pgn move
          4 bytes (32 bits) -> uint32_t: times played
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

    std::cout
        << ColorCode::teal
        << "Writing " << m_tablebase.size() << " key/val pairs."
        << ColorCode::end << std::endl;

    for (auto node = m_tablebase.begin(); node != m_tablebase.end(); node++)
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
      std::cerr << ColorCode::red << "Could not open file: " << file_path << ColorCode::end << std::endl;
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

      uint32_t move_map_size = *((uint32_t *)(data + index));
      index += sizeof(uint32_t);

      std::shared_ptr<std::unordered_map<MoveKey, MoveEdge>> move_map =
          std::make_shared<std::unordered_map<MoveKey, MoveEdge>>();

      for (int m = 0; m < move_map_size; m++)
      {

        MoveKey move_key = *((uint32_t *)(data + index));
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

      deserialized_tablebase.m_tablebase[key_hash] = move_map;
    }

    return deserialized_tablebase;
  }

  static std::vector<OpeningTablebase> read_tablebases(std::string tablebase_filepath)
  {
    std::vector<OpeningTablebase> openingTablebases;

    for (const auto &entry : std::filesystem::directory_iterator(tablebase_filepath))
    {
      std::string filepath_str = entry.path().generic_string();
      if (filepath_str.substr(filepath_str.size() - 3).compare(".tb") != 0)
      {
        continue;
      }

      std::cout
          << ColorCode::purple << filepath_str.substr(filepath_str.find_last_of('/') + 1)
          << ColorCode::end << std::endl;
      openingTablebases.push_back(deserialize_tablebase(entry.path()));
    }
    return openingTablebases;
  }

  static std::shared_ptr<OpeningTablebase> merge_tablebases(
      std::vector<OpeningTablebase> *tablebase_vector)
  {
    std::shared_ptr<OpeningTablebase> merged_tablebase = std::make_shared<OpeningTablebase>();
    auto mtb = &(merged_tablebase->m_tablebase);

    int count = 0;
    std::cout << ColorCode::teal << "Number of tablebases: "
              << tablebase_vector->size() << ColorCode::end << std::endl;

    std::cout
        << std::left << std::setw(20) << "Tablebase Address"
        << std::left << std::setw(20) << "# Positions"
        << std::endl;

    // iterate through the list of tablebases we have read from disk
    for (auto it = tablebase_vector->begin(); it != tablebase_vector->end(); it++)
    // for (auto tablebase : *tablebase_vector)
    {
      auto tablebase = *it;
      std::cout
          << ColorCode::purple << std::left << std::setw(20) << &(*it)
          << ColorCode::teal << std::left << std::setw(20) << tablebase.m_tablebase.size()
          << ColorCode::end << std::endl;

      // if the tablebase we are aggregating is empty, let's set the root hash. The root hash of all of the
      // tablebases should be the same, since all games start from the same position. Could be useful to put
      // an assertion for this somewhere.
      if (mtb->empty())
      {
        merged_tablebase->m_root_hash = tablebase.m_root_hash;
      }

      // merge the move maps of the current tablebase, with our aggregate, for every position
      for (auto pair : tablebase.m_tablebase)
      {
        auto current_position_hash = pair.first;
        auto current_move_map = pair.second;

        // try to find the current position in the aggregate tablebase
        auto position_lookup_result = mtb->find(current_position_hash);

        // if it is found, merge the move maps
        if (position_lookup_result != mtb->end())
        {
          auto aggregated_map = (*mtb)[current_position_hash];

          // iterate through the move map,
          // sum times_played if it is already present in the aggregate,
          // insert move edge otherwise
          for (auto key_move_pair : *current_move_map)
          {
            auto move_lookup_result = aggregated_map->find(key_move_pair.first);
            if (move_lookup_result != aggregated_map->end())
            {
              move_lookup_result->second.m_times_played += key_move_pair.second.m_times_played;
            }
            else
            {
              aggregated_map->insert(key_move_pair);
              // (*aggregated_map)[key_move_pair.first] = key_move_pair.second;
            }
          }
        }
        // position hasnt been seen yet, so just take the current move map
        else
        {
          (*mtb)[current_position_hash] = std::move(current_move_map);
        }
      }
    }

    return merged_tablebase;
  }
};