#pragma once
#include <vector>

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UPLEFT,
    UPRIGHT,
    DOWNLEFT,
    DOWNRIGHT
};

const std::vector<Direction> directions_vector = {
    Direction::UP,
    Direction::DOWN,
    Direction::LEFT,
    Direction::RIGHT,
    Direction::UPLEFT,
    Direction::UPRIGHT,
    Direction::DOWNLEFT,
    Direction::DOWNRIGHT};

const int bishop_offsets[4] = {15, 17, -15, -17};
const int rook_offsets[4] = {16, 1, -16, -1};

#define RANK_OFFSET 16
#define FILE_OFFSET 1

#define NEXT_RANK(sq) (static_cast<uint8_t>(sq + RANK_OFFSET))
#define PREV_RANK(sq) (static_cast<uint8_t>(sq - RANK_OFFSET))

#define PREV_FILE(sq) (static_cast<uint8_t>(sq - FILE_OFFSET))
#define NEXT_FILE(sq) (static_cast<uint8_t>(sq + FILE_OFFSET))

// Returns the rank that is forward relative to the player
#define FORWARD_RANK(C, square) \
    (is_white(C) ? NEXT_RANK(square) : PREV_RANK(square))

#define BACKWARD_RANK(C, square) \
    (is_white(C) ? PREV_RANK(square) : NEXT_RANK(square))

constexpr uint8_t direction_offset(Direction D)
{
    switch (D)
    {
    case Direction::UP:
        return NEXT_RANK(0);
    case Direction::DOWN:
        return PREV_RANK(0);
    case Direction::RIGHT:
        return NEXT_FILE(0);
    case Direction::LEFT:
        return PREV_FILE(0);
    case Direction::UPLEFT:
        return PREV_FILE(NEXT_RANK(0));
    case Direction::UPRIGHT:
        return NEXT_FILE(NEXT_RANK(0));
    case Direction::DOWNLEFT:
        return PREV_FILE(PREV_RANK(0));
    case Direction::DOWNRIGHT:
        return NEXT_FILE(PREV_RANK(0));
    default:
        __builtin_unreachable();
    }
}

const std::vector<int> knight_move_offsets = {
    RANK_OFFSET + RANK_OFFSET + FILE_OFFSET,
    RANK_OFFSET + RANK_OFFSET - FILE_OFFSET,
    -RANK_OFFSET - RANK_OFFSET + FILE_OFFSET,
    -RANK_OFFSET - RANK_OFFSET - FILE_OFFSET,
    FILE_OFFSET + FILE_OFFSET + RANK_OFFSET,
    FILE_OFFSET + FILE_OFFSET - RANK_OFFSET,
    -FILE_OFFSET - FILE_OFFSET + RANK_OFFSET,
    -FILE_OFFSET - FILE_OFFSET - RANK_OFFSET};

#define STEP_DIRECTION(D, square) (direction_offset(D) + square)
