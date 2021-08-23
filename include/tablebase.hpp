#pragma once

#include "read_pgn_data.hpp"
#include "game.hpp"
#include "util.hpp"
#include "position.hpp"
#include <vector>
#include <unordered_map>

struct move_edge
{
    uint64_t dest_hash;
    uint32_t times_played;
    // move key is bit-wise concatenation of
    // 0x00 + start_square + end_square + promotion_piece
    uint32_t move_key;
};

struct OpeningTablebase
{
    std::unordered_map<uint64_t, std::vector<move_edge>> opening_tablebase;

    void process_game(Game *game)
    {
        for (auto it = game->move_list.begin(); it != game->move_list.end(); it++)
        {
            uint32_t move_key = *it;
            Move move = unpack_move_key(move_key);
            std::string src_square_str = index_to_an_square(move.src_square);
            std::string dest_square_str = index_to_an_square(move.dest_square);

            std::cout << *it << std::endl;
            std::cout << src_square_str << " -> " << dest_square_str << std::endl;

            if (move.promotion_piece)
            {

                std::cout << "Promotion piece: " << piece_to_name(move.promotion_piece) << std::endl;
            }
            std::cout << std::endl;
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
