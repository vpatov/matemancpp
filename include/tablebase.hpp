#pragma once

#include "read_pgn_data.hpp"
#include "game.hpp"
#include "util.hpp"
#include "position.hpp"
#include "squares.hpp"
#include "zobrist.hpp"
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <queue>
#include <type_traits>

// LASTLEFTOFF
// multithreading for creation of tablebase.
// every "task" that can be given to a thread, is to create a tablebase for a pgn file.
// the main thread can then take the results from all the other threads and combine them into one
// tablebase.
// Need to figure out way for process_game_move to have reference to tablebase
// Options:
//      1) Make some of the read pgn methods class methods for the opening tablebase
//          (But then how do the game class methods have reference)
//      2) Add a pointer as a parameter
//            (but then you need to pass it a few functions deep)
//      3) Consolidate the Game and OpeningTablebase objects?
//      4) Add pointer to opening tablebase as a field on Game, and set it for each game. (least amount of code)
//            (kind of ugly, not elegant, and feels smelly)
//      5) Give each thread their own pointer to an opening tablebase?
// LASTLEFTOFF
// Okay... we are getting double free errors. serious debugging time
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
  std::string m_pgn_move;

  friend std::ostream &operator<<(std::ostream &os, MoveEdge &move_edge);

  MoveEdge(z_hash_t dest_hash, uint32_t move_key, std::string pgn_move)
  {
    m_dest_hash = dest_hash;
    m_move_key = move_key;
    m_pgn_move = std::move(pgn_move);
    m_times_played = 1;
  };
};
bool compare_move_edge(MoveEdge move_edge1, MoveEdge move_edge2);

struct OpeningTablebase
{
  std::unordered_map<z_hash_t, std::shared_ptr<std::vector<MoveEdge>>> m_tablebase;
  // std::unordered_map<z_hash_t, std::vector<MoveEdge>> m_tablebase;
  z_hash_t m_root_hash;

  // TODO split if/else code paths into two functions
  void update(z_hash_t insert_hash, z_hash_t dest_hash, uint32_t move_key, std::string pgn_move)
  {
    if (m_tablebase.empty())
    {
      m_root_hash = insert_hash;
    }
    // find the node corresponding to the zobrist hash for the position we are inserting at.
    auto node = m_tablebase.find(insert_hash);

    // if
    if (node == m_tablebase.end())
    {
      MoveEdge edge(dest_hash, move_key, pgn_move);
      std::shared_ptr<std::vector<MoveEdge>> move_edge_list = std::make_shared<std::vector<MoveEdge>>();
      move_edge_list->emplace_back(edge);

      m_tablebase[insert_hash] = std::move(move_edge_list);
    }
    else
    {
      // std::cout << "tablebase size: " << m_tablebase.size() << std::endl;
      // std::shared_ptr<std::vector<move_edge>> move_edge_list = std::move(node->second);

      // if the move exists already, just increment the times its been played
      auto it = node->second->begin();
      for (; it != node->second->end(); it++)
      {
        if (it->m_move_key == move_key)
        {
          assert(it->m_dest_hash == dest_hash);
          it->m_times_played++;
          break;
        }
      }

      // if we couldn't find the move in the move list, it's a new move
      if (it == node->second->end())
      {
        MoveEdge edge(dest_hash, move_key, pgn_move);
        node->second->emplace_back(edge);
      }
    }
  }

  void walk_down_most_popular_path()
  {
    auto root = m_tablebase.find(m_root_hash);
    assert(root != m_tablebase.end());

    std::queue<decltype(root)> to_visit;
    to_visit.push(root);

    int depth = 0;

    while (!to_visit.empty())
    {
      auto node = to_visit.front();
      to_visit.pop();
      depth++;

      MoveEdge most_popular_move = *(
          std::max_element(
              node->second->begin(), node->second->end(), &compare_move_edge));

      std::cout << "Depth: " << depth << std::endl;
      std::cout << std::endl
                << most_popular_move << std::endl;

      auto next_node = m_tablebase.find(most_popular_move.m_dest_hash);
      if (next_node != m_tablebase.end())
      {
        to_visit.push(next_node);
      }
    }
  }

  /*
  void try_all_paths()
  {
    typedef std::__1::unordered_map<z_hash_t, std::__1::vector<MoveEdge>>::iterator tablebase_iter;
    auto root = m_tablebase.find(m_root_hash);
    assert(root != m_tablebase.end());

    std::queue<std::pair<tablebase_iter, int>> to_visit;
    to_visit.push(std::make_pair(root, 0));

    while (!to_visit.empty())
    {
      auto pair = to_visit.front();
      to_visit.pop();

      auto node = pair.first;
      int depth = pair.second;

      for (auto move : node->second)
      {
        auto next_node = m_tablebase.find(move.dest_hash);
        if (next_node != m_tablebase.end())
        {
          for (int i = 0; i < depth; i++)
          {
            std::cout << "\t";
          }
          std::cout << move.pgn_move << std::endl;
          to_visit.push(std::make_pair(next_node, depth + 1));
        }
      }

      if (depth == 5)
      {
        return;
      }
    }
  }
  */
};

/*
uint8_t start_square
uint8_t end_square
uint8_t promotion_piece (0 if no promotion)
using move_key = uint32_t (concatenation of above);

struct move {
  uint64_t dest_hash
  uint32_t times_played
}


struct node {

  uint64_t hash (maybe not stored in node)
  position (entire copy?, probably not)

  // hash and position are technically not necessary to store, if
  // we arrive at a position always walking from starting position

  hashmap<move_key, uint64_t> move_map;
  // might be better to use a vector and not a move_map
}


// TODO
// 1) implement next_position(from_square, dest_square, promotion)
// 2) implement get_fromsq_destsq_promotion(playerMove)
// 3) think about threading and locks for this code below
// 4) Think about using regex instead of a parser



  For each game:
    position = new Position in starting position:
    current_node* = opening_tablebase[starting_position]
    If no result or elo below threshold: skip
    For every move in move list:
      key = (from_square, dest_square, promotion)
      move* = current_node.move_map.get(key, NULL);

      position = next_position(key)

      if (move != NULL){
        move->times_played ++;
        currrent_node = opening_tablebase[move.dest_hash];
      }
      else {
        hash = calculate_zobrist_hash(position)
        move = {
          dest_hash : hash,
          times_played : 1
        };
        current_node.move_map.set(key, move);
      }
*/
