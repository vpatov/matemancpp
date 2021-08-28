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

// LASTLEFTOFF
// Design your opening tablebase concretely
// get it such that it includes all the games of one file
// code up bare-bones CLI with good printouts, so that you can play with it
// after that worry about threading for read_pgn_file.

struct MoveEdge
{
  uint64_t dest_hash;
  uint32_t times_played;
  // move key is bit-wise concatenation of
  // 0x00 + start_square + end_square + promotion_piece
  uint32_t move_key;
  std::string pgn_move;

  friend std::ostream &operator<<(std::ostream &os, MoveEdge &move_edge);
};
bool compare_move_edge(MoveEdge move_edge1, MoveEdge move_edge2);

struct OpeningTablebase
{
  // std::unordered_map<z_hash_t, std::shared_ptr<std::vector<move_edge>>> m_tablebase;
  std::unordered_map<z_hash_t, std::vector<MoveEdge>> m_tablebase;
  z_hash_t m_root_hash;

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
      MoveEdge edge;
      edge.dest_hash = dest_hash;
      edge.move_key = move_key;
      edge.times_played = 1;
      edge.pgn_move = std::string(pgn_move);

      // std::shared_ptr<std::vector<move_edge>> move_edge_list = std::make_shared<std::vector<move_edge>>();
      std::vector<MoveEdge> move_edge_list;
      move_edge_list.emplace_back(edge);

      m_tablebase[insert_hash] = std::move(move_edge_list);
    }
    else
    {
      // std::cout << "tablebase size: " << m_tablebase.size() << std::endl;
      // std::shared_ptr<std::vector<move_edge>> move_edge_list = std::move(node->second);

      // if the move exists already, just increment the times its been played
      auto it = node->second.begin();
      for (; it != node->second.end(); it++)
      {
        if (it->move_key == move_key)
        {
          assert(it->dest_hash == dest_hash);
          it->times_played++;
          break;
        }
      }

      // if we couldn't find the move in the move list, it's a new move
      if (it == node->second.end())
      {
        MoveEdge edge;
        edge.dest_hash = dest_hash;
        edge.move_key = move_key;
        edge.times_played = 1;
        edge.pgn_move = pgn_move;
        node->second.emplace_back(edge);
      }
    }
  }

  void walk_down_most_popular_path()
  {
    typedef std::__1::unordered_map<z_hash_t, std::__1::vector<MoveEdge>>::iterator tablebase_iter;
    auto root = m_tablebase.find(m_root_hash);
    assert(root != m_tablebase.end());

    std::queue<tablebase_iter> to_visit;
    to_visit.push(root);

    int depth = 0;

    while (!to_visit.empty())
    {
      auto node = to_visit.front();
      to_visit.pop();
      depth++;

      MoveEdge most_popular_move = *(
          std::max_element(
              node->second.begin(), node->second.end(), &compare_move_edge));

      std::cout << "Depth: " << depth << std::endl;
      std::cout << std::endl
                << most_popular_move << std::endl;

      auto next_node = m_tablebase.find(most_popular_move.dest_hash);
      if (next_node != m_tablebase.end())
      {
        to_visit.push(next_node);
      }
    }
  }

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
