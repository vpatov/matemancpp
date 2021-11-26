

#pragma once

#include <string>
#include "tablebase/tablebase.hpp"
#include "move_generation.hpp"
#include "representation/position.hpp"
#include "engine/evaluation.hpp"

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

    void set_tablebase(std::shared_ptr<Tablebase> tablebase)
    {
        m_master_tablebase = tablebase;
    }

    void set_position(std::shared_ptr<Position> position)
    {
        m_current_position = position;
    }

    MoveKey search_for_best_move()
    {
        int depth = 3;
        std::vector<MoveKey> all_moves = get_all_moves();
        bool whites_turn = m_current_position->m_whites_turn;

        MoveKey best_move = 0;
        double best_score = 0;
        for (auto it = all_moves.begin(); it != all_moves.end(); it++)
        {
            MoveKey movekey = *it;
            Move move = unpack_move_key(movekey);
            std::string lan = movekey_to_lan(movekey);
            auto adjustment = m_current_position->advance_position(unpack_move_key(movekey));
            double score = evaluate(m_current_position);
            if (!best_move || (whites_turn ? (score > best_score) : (score < best_score)))
            {
                best_score = score;
                best_move = movekey;
            }
            m_current_position->undo_adjustment(adjustment);
        }

        std::string res_lan = movekey_to_lan(best_move);
        return best_move;
    }

    MoveKey make_random_move()
    {
        std::vector<MoveKey> all_moves = get_all_moves();
        auto movekey = all_moves.at(random_bitstring() % all_moves.size());
        return movekey;
    }

    std::vector<MoveKey> get_all_moves()
    {
        Color c = m_current_position->m_whites_turn ? Color::WHITE : Color::BLACK;
        square_t square = 0;
        std::vector<MoveKey> all_moves;
        while (square <= H8_SQ)
        {
            if (is_invalid_square(square))
            {
                square += 8;
            }

            if (IS_YOUR_PIECE(c, m_current_position->m_mailbox[square]))
            {
                auto moves = generate_legal_moves(m_current_position, square);
                all_moves.insert(all_moves.end(), moves.begin(), moves.end());
            }
            square++;
        }
        return all_moves;
    }

    std::string string_list_all_moves()
    {
        std::stringstream ss;
        std::vector<MoveKey> all_moves = get_all_moves();
        for (auto m = all_moves.begin(); m != all_moves.end(); m++)
        {
            ss << movekey_to_lan(*m) << " ";
        }
        ss << '\n';
        return ss.str();
    }

    void print_all_moves()
    {
        std::cout << string_list_all_moves() << std::endl;
    }

    MoveKey tablebase_move_lookup()
    {
        MoveKey tablebase_move = VOID_MOVE;
        if (m_master_tablebase != NULL)
        {
            z_hash_t position_hash = zobrist_hash(m_current_position.get());
            tablebase_move = m_master_tablebase->pick_move_from_sample(position_hash);
        }
        return tablebase_move;
    }

    MoveKey find_best_move(std::chrono::milliseconds time)
    {
        MoveKey tablebase_move = tablebase_move_lookup();

        if (tablebase_move)
        {
            return tablebase_move;
        }

        return search_for_best_move();
    }
};
