

#pragma once

#include <string>
#include "tablebase/tablebase.hpp"
#include "move_generation.hpp"
#include "representation/position.hpp"

class Engine
{
public:
    std::shared_ptr<Tablebase> m_master_tablebase;
    std::shared_ptr<Position> m_current_position;

    Engine()
    {
        m_master_tablebase = NULL;
        m_current_position = starting_position();
    }

    // std::string find_best_move(std::chrono::milliseconds time);

    void set_tablebase(std::shared_ptr<Tablebase> tablebase)
    {
        m_master_tablebase = tablebase;
    }

    void set_position(std::shared_ptr<Position> position)
    {
        m_current_position = position;
    }

    // should just return random legal moves at first.
    std::string search_for_best_move()
    {
        // generate legal moves for position
        // for each move
        //      assume the move is made
        //      evaluate the position
        //      minmax
        Color c = m_current_position->m_whites_turn ? Color::WHITE : Color::BLACK;
        square_t square = 0;
        std::vector<MoveKey> all_moves;
        while (square < 120)
        {
            if (is_invalid_square(square))
            {
                square += 8;
            }

            if (IS_YOUR_PIECE(c, m_current_position->m_mailbox[square]))
            {
                auto moves = generate_pseudolegal_piece_moves(m_current_position, square);
                all_moves.insert(all_moves.end(), moves.begin(), moves.end());
            }
            square++;
        }
        auto movekey = all_moves.at(random_bitstring() % all_moves.size());
        auto move = unpack_move_key(movekey);
        return index_to_an_square(move.m_src_square) + index_to_an_square(move.m_dst_square);
    }

    std::string find_best_move(std::chrono::milliseconds time)
    {

        // try to look up position in opening tablebase. if found,
        // pick a move, with the likelihood of a move being selected proportional
        // to how often that move is seen.
        // return in long algebraic notation

        if (m_master_tablebase != NULL)
        {
            z_hash_t position_hash = zobrist_hash(m_current_position.get());
            std::string lookup_move = m_master_tablebase->pick_move_from_sample(position_hash);

            if (lookup_move.size())
            {
                return lookup_move;
            }
        }

        // std::string best_move = search_for_best_move();

        // perform search
        // generate legal moves for position
        // for each move
        //      assume the move is made
        //      evaluate the position
        //      minmax

        // either the tablebase is null or it didnt find the position
        return search_for_best_move();
    }
};
