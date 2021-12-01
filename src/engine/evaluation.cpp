#include "engine/evaluation.hpp"
#include "representation/position.hpp"

int basic_material_for_piece(piece_t piece)
{
    piece_t val = piece & PIECE_MASK;
    switch (val)
    {
    case PAWN:
        return 1;
    case KNIGHT:
    case BISHOP:
        return 3;
    case ROOK:
        return 5;
    case KING:
        return 0;
    case QUEEN:
        return 9;
    default:
        __builtin_unreachable();
    }
}

struct PositionEval
count_material(std::shared_ptr<Position> position)
{
    PositionEval eval;
    square_t square = 0;
    while (square <= H8_SQ)
    {
        if (is_invalid_square(square))
        {
            square += 8;
            continue;
        }
        piece_t piece = position->m_mailbox[square];
        if (piece == VOID_PIECE)
        {
            square++;
            continue;
        }
        if (is_white_piece(piece))
        {
            eval.white_material += basic_material_for_piece(piece);
        }
        else if (is_black_piece(piece))
        {
            eval.black_material += basic_material_for_piece(piece);
        }
        else
        {
            assert(false);
        }
        square++;
    }
    return eval;
}

// LASTLEFTOFF
// need to encode checkmate, king safety, number of squares being controlled, into evaluation

bool is_checkmate(std::shared_ptr<Position> position)
{
    return false;
}

// negative is good for black, positive is good for white
int evaluate(std::shared_ptr<Position> position)
{
    auto material_eval = count_material(position);
    return material_eval.white_material - material_eval.black_material;
}
