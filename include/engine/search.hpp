#pragma once
#include "engine/evaluation.hpp"
#include "engine/engine.hpp"
#include "representation/move.hpp"
#include "representation/position.hpp"
#include "move_generation.hpp"

// fen to test
// r7/8/k7/3N4/8/PK5P/8/8 w - - 0 1
// best move should be d5c7

/// TODO consider passing to this function a copy rather than a pointer
MoveKey minmax_search(std::shared_ptr<Position> position)
{
    // depth of the position we start search from is 0.
    // depth of a move = depth of position in which it was made.
    using depth = size_t;

    std::set<z_hash_t> unique_positions;
    z_hash_t starting_hash = zobrist_hash(position.get());

    size_t current_depth = 0;
    int max_depth = 4;

    long nodes_visited = 0;

    // the move_stack consists of <depth, movekey>.
    std::vector<std::pair<depth, MoveKey>> move_stack;
    std::vector<PositionAdjustment> adjustment_stack;

    move_stack.reserve(max_depth * 50);
    adjustment_stack.reserve(max_depth);

    auto starting_moves = Engine::get_all_moves(position);
    for (auto it = starting_moves.begin(); it != starting_moves.end(); it++)
    {
        move_stack.push_back(std::make_pair(current_depth, *it));
    }

    while (!move_stack.empty())
    {
        auto pair = move_stack.back();
        depth move_depth = pair.first;
        MoveKey movekey = pair.second;
        move_stack.pop_back();

        unique_positions.insert(zobrist_hash(position.get()));
        nodes_visited++;

        // if the move we popped off isnt at the same depth as us
        // then we need to undo moves to get back (DFS)
        while (current_depth > move_depth)
        {
            position->undo_adjustment(adjustment_stack.back());
            adjustment_stack.pop_back();
            current_depth--;

            // right after we are done exploring the last move for a node,
            // this loop will run twice because we will undo the last move,
            // and also undo the move that got us to the node.
            // the thing is, this is the moment when it is necessary to minmax.
            // the solutions i have are all a little bit ugly, which worries me
            // because this isnt turning out elegant, and it remains sort of complex
            // (i've been trying to simplify this)

            /*
            // my options are: 
            1)
            // undo_adjustment twice, and then have a condition outside the loop
            if (current_depth == top_of_eval_stack_depth - 2){
                min,max = minmax(eval_stack)
                // push either min or max onto the eval_stack, depending on whose turn
                // but then when we push onto eval_stack, how do we know what move
                // we just analyzed...
            }

            2)
            // before we roll back a position, check if
            // our current position_depth == top_of_eval - 1
            // if so, pop all evals from the stack, minmax them, and push
            // one value onto the stack.
            // (this seems a bit better than option #1?)
            */
        }

        // save the adjustment struct so we can undo this move later
        adjustment_stack.push_back(position->advance_position(movekey));
        current_depth++;

        // if not at the leaf yet, go deeper
        if (current_depth < max_depth)
        {
            std::vector<MoveKey> node_moves = Engine::get_all_moves(position);
            for (auto it = node_moves.begin(); it != node_moves.end(); it++)
                move_stack.push_back(std::make_pair(current_depth, *it));
        }

        // if at the leaf, evaluate the position and store the result. parent nodes will minmax
        else
        {
            // int score = evaluate(position);
        }
    }

    // unroll any adjustments to get back to where we came from.
    while (!adjustment_stack.empty())
    {
        auto adjustment = adjustment_stack.back();
        position->undo_adjustment(adjustment);
        adjustment_stack.pop_back();

        current_depth--;
    }
    assert(zobrist_hash(position.get()) == starting_hash);

    std::cout << "nodes visited: " << nodes_visited << std::endl;
    std::cout << "unique positions: " << unique_positions.size() << std::endl;
    return 0;
}