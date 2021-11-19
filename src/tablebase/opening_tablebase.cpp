#include "tablebase/opening_tablebase.hpp"
#include "util.hpp"
#include <set>
#include <sstream>

/* 
  This function calculates and returns the move key, which is a concatenation 
  of the source square, destination square, and promotion piece (complete 
  information necessary to understand a move). For castling moves, even though
  two pieces move, the src_square and dest_square pertain only to the king.
*/
uint32_t generate_move_key(uint8_t src_square, uint8_t dest_square, uint8_t promotion_piece)
{
    // move key is bit-wise concatenation of
    // (empty/reserved) + start_square + end_square + promotion_piece
    // 8 bits             8 bits         8 bits       8 bits
    return (src_square << 16) + (dest_square << 8) + promotion_piece;
}

// | 00 | start_square | end_square | promotion_piece

std::string generate_long_algebraic_notation(MoveKey move_key)
{
    std::stringstream ss;
    ss << index_to_an_square((move_key >> 16) & 0xff);
    ss << index_to_an_square((move_key >> 8) & 0xff);
    piece_t promotion_piece = move_key & 0xff;

    if (promotion_piece)
    {
        ss << "=" << piece_to_char(promotion_piece);
    }
    return ss.str();
}

bool compare_key_move_pair(std::pair<MoveKey, MoveEdge> p1, std::pair<MoveKey, MoveEdge> p2)
{
    if (p1.second.m_times_played < p2.second.m_times_played)
    {
        return true;
    }
    else if (p1.second.m_times_played == p2.second.m_times_played)
    {
        return p1.second.m_dest_hash < p2.second.m_dest_hash;
    }
    else
    {
        return false;
    }
}

std::ostream &operator<<(std::ostream &os, MoveEdge &move_edge)
{
    // Move move = unpack_move_key(move_edge.m_move_key);
    os
        << "dest_hash: " << move_edge.m_dest_hash << std::endl
        << "pgn_move: " << move_edge.m_pgn_move << std::endl
        << "times_played: " << move_edge.m_times_played << std::endl
        // << "move_key: " << move_edge.m_move_key << std::endl
        << std::endl;

    return os;
}

void OpeningTablebase::insert_new_move_map(z_hash_t insert_hash, MoveKey moveKey, MoveEdge moveEdge)
{
    std::shared_ptr<std::unordered_map<MoveKey, MoveEdge>> move_map =
        std::make_shared<std::unordered_map<MoveKey, MoveEdge>>();
    move_map->insert(std::pair(moveKey, moveEdge));

    m_tablebase[insert_hash] = std::move(move_map);
}

void OpeningTablebase::increment_times_played_or_insert_move(
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

void OpeningTablebase::update(z_hash_t insert_hash, z_hash_t dest_hash, MoveKey move_key, std::string pgn_move)
{
    if (m_tablebase.empty())
    {
        m_root_hash = insert_hash;
    }
    MoveEdge moveEdge(dest_hash, pgn_move);

    // find the node corresponding to the zobrist hash for the position we are inserting at.
    auto node = m_tablebase.find(insert_hash);

    if (node == m_tablebase.end())
    {
        insert_new_move_map(insert_hash, move_key, moveEdge);
    }
    else
    {
        increment_times_played_or_insert_move(node, move_key, moveEdge);
    }

    // if (insert_hash == 1113003689047388558)
    // {
    //     std::cout << ColorCode::red << "insert hash is equal to special value. pgn move: "
    //               << pgn_move << ColorCode::end << std::endl;
    // }

    // if (dest_hash == 1113003689047388558)
    // {
    //     std::cout << ColorCode::red << "dest hash is equal to special value. pgn move: "
    //               << pgn_move << ColorCode::end << std::endl;
    // }
}

void OpeningTablebase::walk_down_most_popular_path(std::string file_path)
{
    std::ofstream out(file_path);

    auto root = m_tablebase.find(m_root_hash);
    assert(root != m_tablebase.end());

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

        for (auto it = node->second->begin(); it != node->second->end(); it++)
        {
            auto foo = *it;
        }

        auto key_move_pair = *(
            std::max_element(
                node->second->begin(), node->second->end(), &compare_key_move_pair));

        MoveKey most_popular_move_key = key_move_pair.first;
        MoveEdge *most_popular_move = &key_move_pair.second;

        out << "Depth: " << depth << std::endl;
        out << most_popular_move_key << std::endl;
        out << *most_popular_move << std::endl;

        auto next_node = m_tablebase.find(most_popular_move->m_dest_hash);
        if (next_node != m_tablebase.end())
        {
            to_visit.push(next_node);
        }
    }
    out.close();
}

void OpeningTablebase::walk_down_most_popular_path()
{

    auto root = m_tablebase.find(m_root_hash);
    assert(root != m_tablebase.end());

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

        auto next_node = m_tablebase.find(most_popular_move->m_dest_hash);
        if (next_node != m_tablebase.end())
        {
            to_visit.push(next_node);
        }
    }
}

std::string OpeningTablebase::pick_move_from_sample(z_hash_t position_hash)
{

    auto node = m_tablebase.find(position_hash);

    if (node == m_tablebase.end())
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