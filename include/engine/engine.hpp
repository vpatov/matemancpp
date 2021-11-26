

#pragma once

#include <string>
#include <stack>
#include "tablebase/tablebase.hpp"
#include "move_generation.hpp"
#include "representation/position.hpp"
#include "representation/fen.hpp"
#include "engine/evaluation.hpp"
#include <algorithm>
#include <cstdlib>
#include <limits>

class Engine
{
public:
    std::shared_ptr<Tablebase> m_master_tablebase;
    std::shared_ptr<Position> m_current_position;
    std::mt19937 m_g;

    Engine()
    {
        m_master_tablebase = NULL;
        m_current_position = starting_position();
        std::random_device rd;
        m_g = std::mt19937(rd());
    }

    void set_tablebase(std::shared_ptr<Tablebase> tablebase)
    {
        m_master_tablebase = tablebase;
    }

    void set_position(std::shared_ptr<Position> position)
    {
        m_current_position = position;
    }

    // void minmax(std::tuple<int, MoveKey, double> *eval_stack)
    // {
    //     // ------- MINMAX
    //     double min_score =
    //         std::numeric_limits<float>::max();
    //     MoveKey min_move;
    //     double max_score =
    //         std::numeric_limits<float>::min();
    //     MoveKey max_move;

    //     while (std::get<0>(eval_stack->back()) > current_depth)
    //     {
    //         auto eval = eval_stack->back();
    //         eval_stack->pop_back();
    //         MoveKey move = std::get<1>(eval);
    //         double score = std::get<2>(eval);
    //         if (score < min_score)
    //         {
    //             min_score = score;
    //             min_move = move;
    //         }
    //         else if (score > max_score)
    //         {
    //             max_score = score;
    //             max_move = move;
    //         }
    //     }
    // }

    std::pair<int, MoveKey> best_move_material_dn_r(std::shared_ptr<Position> position, bool for_white, int max_depth, int cur_depth)
    {

        auto moves = get_all_moves(position);

        bool score_set = false;
        int best_score = 0;
        MoveKey best_move = 0;
        std::string best_move_string;

        assert(max_depth % 2 != 0);

        for (auto it = moves.begin(); it != moves.end(); it++)
        {
            auto movekey = *it;
            std::shared_ptr<Position> new_position = std::make_shared<Position>(*position);
            std::string lanmove = movekey_to_lan(movekey);
            new_position->advance_position(movekey);

            if (movekey == pack_move_key(E5_SQ, F7_SQ) && cur_depth == 0)
            {
                int x = 5;
            }
            else if (movekey == pack_move_key(F7_SQ, D8_SQ))
            {
                int y = 6;
            }

            // evaluate at leaves
            if (cur_depth == max_depth)
            {
                int score = evaluate(new_position);
                // at max depth we are looking for what is best for black
                if (!score_set || (for_white ? (score < best_score) : (score > best_score)))
                {
                    best_score = score;
                    score_set = true;
                    best_move = movekey;
                    best_move_string = movekey_to_lan(best_move);
                }
            }
            else
            {
                auto pair = best_move_material_dn_r(new_position, for_white, max_depth, cur_depth + 1);
                auto score = pair.first;
                auto move = pair.second;

                bool minimize = for_white == (cur_depth % 2 == 0);
                // if (for_white){
                //     minimize = cur_depth % 2 = 0;
                // }
                // else {
                //     minimize = cur_depth % 2 != 0;
                // }

                // if the move we just made in the new position is for black
                // we are comparing them from the perspective of white
                // assume black will make the best move
                if (!score_set || (minimize ? (score < best_score) : (score > best_score)))
                {
                    best_score = score;
                    score_set = true;
                    best_move = movekey;
                }
            }
        }

        return std::make_pair(best_score, best_move);
    }

    MoveKey best_move_material_dn_r_helper(int max_depth)
    {
        auto pair = best_move_material_dn_r(
            m_current_position, m_current_position->m_whites_turn, max_depth, 0);
        return pair.second;
    }

    MoveKey best_move_material_dn(int max_depth)
    {
        size_t current_depth = 0;

        // depth should be even, because we should end our search
        // by evaluating positions from opponent moves.
        // if we could end search from evaluation of our move,
        // then we would reward unjustified piece sacrifices.
        // assert(max_depth % 2 == 0);

        auto all_moves = get_all_moves();

        // the move_stack consists of <depth, movekey>.
        using depth = int;
        std::vector<std::pair<depth, MoveKey>> move_stack;
        move_stack.push_back(std::make_pair(0, 0));

        // for (auto it = all_moves.begin(); it < all_moves.end(); it++)
        // {
        //     move_stack.push_back(std::make_pair(current_depth, *it));
        // }
        std::vector<PositionAdjustment> adjustment_stack;

        // an element here represents its depth, move, and score.
        std::vector<std::tuple<int, MoveKey, int>> eval_stack;

        while (!move_stack.empty())
        {
            auto pair = move_stack.back();
            auto move_depth = pair.first;
            auto move = pair.second;
            move_stack.pop_back();

            // means we have reached the root, pop off evals and find best

            // if the move we popped off isnt at the same depth as us
            // then we need to undo moves to get back (DFS)
            while (current_depth > move_depth)
            {

                // everything on the eval stack is for the children of the node
                // we are about to leave (go back up in DFS)

                // ------- MINMAX
                int min_score =
                    std::numeric_limits<int>::max();
                MoveKey min_move = 0;
                int max_score =
                    std::numeric_limits<int>::min();
                MoveKey max_move = 0;
                bool popped = false;

                while (std::get<0>(eval_stack.back()) > current_depth)
                {
                    popped = true;
                    auto eval = eval_stack.back();
                    eval_stack.pop_back();
                    MoveKey move = std::get<1>(eval);
                    int score = std::get<2>(eval);
                    if (score < min_score)
                    {
                        min_score = score;
                        min_move = move;
                    }
                    else if (score > max_score)
                    {
                        max_score = score;
                        max_move = move;
                    }
                }

                // if its black moves we just evaluated, assume black will pick the best one for them
                // so pick the move with the lowest score
                // if we just evaluated the black responses to a white move, and are about to
                // unroll the white move, then we pick the move with the lowest score
                if (popped)
                {
                    assert(max_move);
                    assert(min_move);
                    eval_stack.push_back(m_current_position->m_whites_turn
                                             ? std::make_tuple(current_depth, min_move, min_score)
                                             : std::make_tuple(current_depth, max_move, max_score));
                }

                if (adjustment_stack.empty())
                {
                    break;
                }

                auto adjustment = adjustment_stack.back();
                adjustment_stack.pop_back();
                m_current_position->undo_adjustment(adjustment);

                for (int i = 0; i < current_depth; i++)
                {
                    std::cout << "\t";
                }
                std::cout << "undid move "
                          << movekey_to_lan(
                                 pack_move_key(
                                     adjustment.src_square,
                                     adjustment.dst_square))
                          << std::endl;

                current_depth--;
            }

            // null move is first move to allow us to not have to generate moves outside the while loop
            if (move)
            {
                // save the adjustment struct so we can undo this move later
                adjustment_stack.push_back(m_current_position->advance_position(move));
                current_depth++;
            }
            for (int i = 0; i < current_depth; i++)
                std::cout << "\t";
            if (current_depth)
                std::cout << "made move " << movekey_to_lan(move) << std::endl;
            else
                std::cout << "starting fen: " << position_to_fen(m_current_position) << std::endl;

            // if not at the leaf yet, go deeper
            if (current_depth < max_depth)
            {
                std::vector<MoveKey> node_moves = get_all_moves();
                for (auto it = node_moves.begin(); it != node_moves.end(); it++)
                {
                    move_stack.push_back(std::make_pair(current_depth, *it));
                }
            }

            // if at the leaf, evaluate the position and store the result. parent nodes will minmax
            else
            {
                // (from white's perspective, assuming we're looking for whites best move)
                // while we're exploring the leaves of one branch,
                // we should keep note of the lowest score,
                // and assume that our opponent will make that move.
                // whichever branch maximizes that score, is the one we pick.
                // whichever branch minimizes that score, is the one our opponent will pick.

                int score = evaluate(m_current_position);
                eval_stack.push_back(std::make_tuple(current_depth, move, score));
            }
        }

        std::cout << eval_stack.size() << std::endl;

        int min_score =
            std::numeric_limits<int>::max();
        MoveKey min_move;
        int max_score =
            std::numeric_limits<int>::min();
        MoveKey max_move;

        while (!eval_stack.empty())
        {
            // expect all the moves at this point to be at depth 0 (or 1 ??)
            auto eval = eval_stack.back();
            eval_stack.pop_back();

            MoveKey move = std::get<1>(eval);
            int score = std::get<2>(eval);
            std::string lanmove = movekey_to_lan(move);
            if (score < min_score)
            {
                min_score = score;
                min_move = move;
            }
            else if (score > max_score)
            {
                max_score = score;
                max_move = move;
            }
        }
        // for (auto el : eval_stack)
        // {
        //     std::cout << std::get<0>(el) << "-> " << std::get<1>(el) << std::endl;
        //     // std::cout
        // }
        // auto move = eval_stack
        // return eval_stack.
        return m_current_position->m_whites_turn ? max_move : min_move;

        // std::stack<std::pair<PositionAdjustment, std::vector<MoveKey>>> search_stack;
        // search_stack.push(std::make_pair<PositionAdjustment, std::vector<MoveKey>>({}, {}));
    }

    // problem is that if moves have even score, it always picks the same one.
    // always picks the first move
    MoveKey best_move_material_d1()
    {

        std::vector<MoveKey> all_moves = get_all_moves();
        std::shuffle(all_moves.begin(), all_moves.end(), m_g);
        bool whites_turn = m_current_position->m_whites_turn;

        MoveKey best_move = 0;
        int best_score = 0;
        for (auto it = all_moves.begin(); it != all_moves.end(); it++)
        {
            MoveKey movekey = *it;
            Move move = unpack_move_key(movekey);
            std::string lan = movekey_to_lan(movekey);
            auto adjustment = m_current_position->advance_position(unpack_move_key(movekey));
            int score = evaluate(m_current_position);
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
        return get_all_moves(m_current_position);
    }

    std::vector<MoveKey> get_all_moves(std::shared_ptr<Position> position)
    {
        Color c = position->m_whites_turn ? Color::WHITE : Color::BLACK;
        square_t square = 0;
        std::vector<MoveKey> all_moves;
        while (square <= H8_SQ)
        {
            if (is_invalid_square(square))
            {
                square += 8;
            }

            if (IS_YOUR_PIECE(c, position->m_mailbox[square]))
            {
                auto moves = generate_legal_moves(position, square);
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
        // MoveKey tablebase_move = tablebase_move_lookup();

        // if (tablebase_move)
        // {
        //     return tablebase_move;
        // }

        return best_move_material_dn(2);
        // return best_move_material_dn_r_helper(3);
        // return best_move_material_dn(4);
        // return best_move_material_dn(4);
        // return best_move_material_d1();
    }
};
