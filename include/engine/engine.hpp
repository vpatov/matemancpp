

#pragma once

#include <string>
#include "tablebase/tablebase.hpp"
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

        // either the tablebase is null or it didnt find the position
        return "h9a9";
    }
};
