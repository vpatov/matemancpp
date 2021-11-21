#include "tablebase/tablebase.hpp"

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

// TODO test this
bool Tablebase::position_exists(z_hash_t position_hash)
{
    return (*this)[position_hash] != NULL;
}

void Tablebase::update(z_hash_t insert_hash, z_hash_t dest_hash, MoveKey move_key, std::string pgn_move)
{
    uint16_t shard = insert_hash % TABLEBASE_SHARD_COUNT;
    std::unique_lock<std::mutex> lock(mutexes[shard]);

    auto position_moveset_map = shards[shard];

    MoveEdge moveEdge(dest_hash, pgn_move);

    // find the node corresponding to the zobrist hash for the position we are inserting at.
    auto node = position_moveset_map.find(insert_hash);

    if (node == position_moveset_map.end())
    {
        insert_new_move_map(insert_hash, move_key, moveEdge);
    }
    else
    {
        increment_times_played_or_insert_move(node, move_key, moveEdge);
    }
}

void Tablebase::insert_new_move_map(z_hash_t insert_hash, MoveKey moveKey, MoveEdge moveEdge)
{
    std::shared_ptr<std::unordered_map<MoveKey, MoveEdge>> move_map =
        std::make_shared<std::unordered_map<MoveKey, MoveEdge>>();
    move_map->insert(std::pair(moveKey, moveEdge));
    uint8_t shard = insert_hash % TABLEBASE_SHARD_COUNT;
    shards[shard][insert_hash] = move_map;
}

void Tablebase::increment_times_played_or_insert_move(
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

std::string Tablebase::pick_move_from_sample(z_hash_t position_hash)
{
    uint16_t shard = position_hash % TABLEBASE_SHARD_COUNT;

    auto position_moveset_map = shards[shard];
    auto node = position_moveset_map.find(position_hash);

    if (node == position_moveset_map.end())
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

void Tablebase::walk_down_most_popular_path()
{
    std::cout << ColorCode::purple << "root hash: " << m_root_hash << ColorCode::end << std::endl;
    uint16_t shard = m_root_hash % TABLEBASE_SHARD_COUNT;
    auto root = shards[shard].find(m_root_hash);
    assert(root != shards[shard].end());

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

        uint8_t next_shard = most_popular_move->m_dest_hash % TABLEBASE_SHARD_COUNT;
        auto next_node = shards[next_shard].find(most_popular_move->m_dest_hash);
        if (next_node != shards[next_shard].end())
        {
            to_visit.push(next_node);
        }
    }
}

int Tablebase::total_size()
{
    int s = 0;
    for (int shard = 0; shard < TABLEBASE_SHARD_COUNT; shard++)
    {
        s += shards[shard].size();
    }
    return s;
}