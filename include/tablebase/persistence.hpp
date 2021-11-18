#pragma once
#include <string>
#include <filesystem>
#include <set>

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