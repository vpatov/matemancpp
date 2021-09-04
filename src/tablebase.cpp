#include "tablebase.hpp"
#include "util.hpp"
#include <set>

bool compare_move_edge(MoveEdge move_edge1, MoveEdge move_edge2)
{
    return move_edge1.m_times_played < move_edge2.m_times_played;
}

std::ostream &operator<<(std::ostream &os, MoveEdge &move_edge)
{
    Move move = unpack_move_key(move_edge.m_move_key);
    os
        << "dest_hash: " << move_edge.m_dest_hash << std::endl
        << "pgn_move: " << move_edge.m_pgn_move << std::endl
        << "times_played: " << move_edge.m_times_played << std::endl
        << "move_key: " << move_edge.m_move_key << std::endl
        << std::endl;

    return os;
}

void OpeningTablebase::insert_new_move_edge_list(z_hash_t insert_hash, MoveEdge moveEdge)
{
    std::shared_ptr<std::vector<MoveEdge>> move_edge_list = std::make_shared<std::vector<MoveEdge>>();
    move_edge_list->emplace_back(moveEdge);

    m_tablebase[insert_hash] = std::move(move_edge_list);
}

void OpeningTablebase::increment_times_played_or_insert_move(
    tablebase_iter node, MoveEdge moveEdge)
{
    // if the move exists already, just increment the times its been played
    auto it = node->second->begin();
    for (; it != node->second->end(); it++)
    {
        if (it->m_move_key == moveEdge.m_move_key)
        {
            assert(it->m_dest_hash == moveEdge.m_dest_hash);
            it->m_times_played++;
            break;
        }
    }

    // if we couldn't find the move in the move list, it's a new move
    if (it == node->second->end())
    {
        node->second->emplace_back(moveEdge);
    }
}

void OpeningTablebase::update(z_hash_t insert_hash, z_hash_t dest_hash, uint32_t move_key, std::string pgn_move)
{
    if (m_tablebase.empty())
    {
        m_root_hash = insert_hash;
    }
    MoveEdge moveEdge(dest_hash, move_key, pgn_move);

    // find the node corresponding to the zobrist hash for the position we are inserting at.
    auto node = m_tablebase.find(insert_hash);

    if (node == m_tablebase.end())
    {
        insert_new_move_edge_list(insert_hash, moveEdge);
    }
    else
    {
        increment_times_played_or_insert_move(node, moveEdge);
    }
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