#pragma once
#include "engine/evaluation.hpp"
#include "engine/engine.hpp"
#include "representation/move.hpp"
#include "representation/position.hpp"
#include "move_generation.hpp"

using depth = size_t;

struct Eval
{
    int m_score;
    MoveKey m_movekey;
    depth m_resultant_position_depth;
    std::string lan_move;

    Eval(int score, MoveKey movekey, depth position_depth)
        : m_score(score), m_movekey(movekey), m_resultant_position_depth(position_depth)
    {
        lan_move = movekey_to_lan(m_movekey);
    }
};

/*
    eval_stack - pointer to the stack that is being used in the minmax method
    current_position_depth - used to determine when to stop popping off the eval stack
    whites_turn - used to determine if we are looking for min or for max
    origin_movekey - the movekey that brought us to this position. this movekey
        is then pushed back onto the eval stack, because the result of minmaxing the moves
        is then used for this node (i.e. e2e4 is evaluated by the minmax of the possible moves after e2e4)
*/
void consolidate_eval_stack(std::vector<std::shared_ptr<Eval>> *eval_stack,
                            depth current_position_depth, bool whites_turn,
                            MoveKey origin_movekey)
{
    std::shared_ptr<Eval> min_eval;
    std::shared_ptr<Eval> max_eval;
    while (!eval_stack->empty()
               ? (current_position_depth == eval_stack->back()->m_resultant_position_depth - 1)
               : false)
    {
        auto eval = eval_stack->back();
        eval_stack->pop_back();

        if (min_eval == NULL && max_eval == NULL)
        {
            min_eval = eval;
            max_eval = eval;
        }
        else if (eval->m_score < min_eval->m_score)
        {
            min_eval = eval;
        }
        else if (eval->m_score > max_eval->m_score)
        {
            max_eval = eval;
        }
    }
    if (min_eval != NULL && max_eval != NULL)
    {
        Eval *eval = whites_turn ? max_eval.get() : min_eval.get();
        eval_stack->push_back(std::make_shared<Eval>(
            Eval(eval->m_score, origin_movekey != 0 ? origin_movekey : eval->m_movekey, eval->m_resultant_position_depth - 1)));
    }
}

/// TODO consider passing to this function a copy rather than a pointer
MoveKey
minmax_search(std::shared_ptr<Position> position, depth max_depth)
{
    // depth of the position we start search from is 0.
    // depth of a move = depth of position in which it was made.
    std::set<z_hash_t> unique_positions;
    z_hash_t starting_hash = zobrist_hash(position.get());

    size_t current_position_depth = 0;

    long nodes_visited = 0;

    // the move_stack consists of <depth, movekey>.
    std::vector<std::pair<depth, MoveKey>> move_stack;
    std::vector<PositionAdjustment> adjustment_stack;
    std::vector<std::shared_ptr<Eval>> eval_stack;

    move_stack.reserve(max_depth * 50);
    adjustment_stack.reserve(max_depth);

    // initialize stack of moves
    auto starting_moves = Engine::get_all_moves(position);
    for (auto it = starting_moves.begin(); it != starting_moves.end(); it++)
    {
        move_stack.push_back(std::make_pair(current_position_depth, *it));
    }

    while (!move_stack.empty())
    {
        auto pair = move_stack.back();
        depth move_depth = pair.first;
        MoveKey movekey = pair.second;
        std::string lanmove = movekey_to_lan(movekey);
        move_stack.pop_back();

        unique_positions.insert(zobrist_hash(position.get()));
        nodes_visited++;

        // if the move we popped off isnt at the same depth as us
        // then we need to undo moves to get back (DFS)
        while (current_position_depth > move_depth)
        {
            consolidate_eval_stack(
                &eval_stack,
                current_position_depth,
                position->m_whites_turn,
                adjustment_stack.back().movekey);
            position->undo_adjustment(adjustment_stack.back());
            adjustment_stack.pop_back();
            current_position_depth--;
        }

        // save the adjustment struct so we can undo this move later
        adjustment_stack.push_back(position->advance_position(movekey));
        current_position_depth++;

        // if not at the leaf yet, go deeper
        if (current_position_depth < max_depth)
        {
            std::vector<MoveKey> node_moves = Engine::get_all_moves(position);
            for (auto it = node_moves.begin(); it != node_moves.end(); it++)
                move_stack.push_back(std::make_pair(current_position_depth, *it));
        }

        // if at the leaf, evaluate the position and store the result. parent nodes will minmax
        else
        {
            int score = evaluate(position);
            eval_stack.push_back(std::make_shared<Eval>(Eval(score, movekey, current_position_depth)));
        }
    }

    // unroll any adjustments to get back to where we came from.
    while (!adjustment_stack.empty())
    {
        consolidate_eval_stack(
            &eval_stack,
            current_position_depth,
            position->m_whites_turn,
            adjustment_stack.back().movekey);
        auto adjustment = adjustment_stack.back();
        position->undo_adjustment(adjustment);
        adjustment_stack.pop_back();

        current_position_depth--;
    }
    assert(current_position_depth == 0);
    assert(zobrist_hash(position.get()) == starting_hash);

    consolidate_eval_stack(&eval_stack, current_position_depth, position->m_whites_turn, 0);
    assert(eval_stack.size() == 1);
    auto eval = eval_stack.back();

    return eval->m_movekey;
}