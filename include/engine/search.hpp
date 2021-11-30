#pragma once
#include "engine/evaluation.hpp"
#include "representation/move.hpp"
#include "representation/position.hpp"
#include "move_generation.hpp"
#include "tablebase/zobrist.hpp"
#include <set>

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
                            MoveKey origin_movekey);

MoveKey
minmax_search(std::shared_ptr<Position> position, depth max_depth);
