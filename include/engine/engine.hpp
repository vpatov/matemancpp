

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
        std::vector<MoveKey> all_moves = get_all_moves();
        auto movekey = all_moves.at(random_bitstring() % all_moves.size());
        auto move = unpack_move_key(movekey);
        std::stringstream ss;
        if (move.m_promotion_piece)
        {
            ss << "=" << piece_to_char(move.m_promotion_piece);
        }
        return index_to_an_square(move.m_src_square) + index_to_an_square(move.m_dst_square) + ss.str();
    }

    std::vector<MoveKey> get_all_moves()
    {
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
            auto move = unpack_move_key(*m);

            ss << index_to_an_square(move.m_src_square)
               << index_to_an_square(move.m_dst_square);
            if (move.m_promotion_piece)
            {
                ss << "=" << piece_to_char(move.m_promotion_piece);
            }
            ss << " ";
        }
        ss << '\n';
        return ss.str();
    }

    // LASTLEFTOFF
    // for some reason, when playing through cutechess, the engine makes a move for white when it should
    // make a move for black
    // looks like it was a threading issue
    // TODO
    // implement fen
    // 0) write some god damn tests
    // 1) print out fen
    // 2) position from fen
    // 2a) test fen for some positions
    // 3) log fen string after every position change
    // 4) use fen string debug situation where illegal move was given
    // 5) clean up code
    //      - make small functions
    //      - separate into interfaes
    //      - make debugging functions clean
    //      - clean up cli
    //      - pgn move processing
    void print_all_moves()
    {
        std::cout << string_list_all_moves() << std::endl;
    }

    std::string tablebase_move_lookup()
    {
        if (m_master_tablebase != NULL)
        {
            z_hash_t position_hash = zobrist_hash(m_current_position.get());
            std::string lookup_move = m_master_tablebase->pick_move_from_sample(position_hash);

            if (lookup_move.size())
            {
                return lookup_move;
            }
        }
        return "";
    }

    std::string find_best_move(std::chrono::milliseconds time)
    {
        // std::string tablebase_move = tablebase_move_lookup();
        // if (tablebase_move.size())
        // {
        //     return tablebase_move;
        // }

        return search_for_best_move();
    }
};
