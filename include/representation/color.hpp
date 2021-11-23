#pragma once
enum class Color
{
    WHITE,
    BLACK
};

constexpr bool
is_white(Color C)
{
    return C == Color::WHITE;
}
