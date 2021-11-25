
#include "representation/notation.hpp"

square_t an_square_to_index(std::string square)
{
    return (square.at(0) - 'a') + ((square.at(1) - '1') * 0x10);
}

square_t an_square_to_index(char src_file, char src_rank)
{
    return (src_file - 'a') + ((src_rank - '1') * 0x10);
}

std::string index_to_an_square(square_t index)
{
    return std::string() + (char)((index % 0x10) + 'a') +
           (char)((index / 0x10) + '1');
}

char index_to_an_file(square_t index)
{
    return (char)((index % 0x10) + 'a');
}

char index_to_an_rank(square_t index)
{
    return (char)((index / 0x10) + '1');
}

piece_t char_to_piece(char piece)
{
    bool white = piece >= 'A' && piece <= 'Z';
    switch (std::toupper(piece))
    {
    case PAWN_CHAR:
        return white ? W_PAWN : B_PAWN;
    case ROOK_CHAR:
        return white ? W_ROOK : B_ROOK;
    case KNIGHT_CHAR:
        return white ? W_KNIGHT : B_KNIGHT;
    case BISHOP_CHAR:
        return white ? W_BISHOP : B_BISHOP;
    case QUEEN_CHAR:
        return white ? W_QUEEN : B_QUEEN;
    case KING_CHAR:
        return white ? W_KING : B_KING;
    default:
        return 0;
    }
}

char piece_to_char(piece_t piece) { return PIECE_CHAR_MAP[piece]; }

const std::string piece_to_name(piece_t piece)
{

    switch (piece & PIECE_MASK)
    {
    case PAWN:
    {
        return "PAWN";
    }
    case ROOK:
    {
        return "ROOK";
    }
    case KNIGHT:
    {
        return "KNIGHT";
    }
    case BISHOP:
    {
        return "BISHOP";
    }
    case QUEEN:
    {
        return "QUEEN";
    }
    case KING:
    {
        return "KING";
    }
    default:
    {
        return "INVALID_PIECE";
    }
    }
}

std::string piece_to_color_coded_char(piece_t piece, bool highlight)
{
    // char chr = PIECE_CHAR_MAP[piece];
    // with color output, all of them can be made uppercase
    char chr = PIECE_CHAR_MAP[piece & PIECE_MASK];
    bool white = is_white_piece(piece);

    std::string white_color = "\u001b[38;5;231";
    std::string black_color = "\u001b[38;5;94";
    std::string highlight_code = ";48;5;";
    std::string highlight_color = "233";
    std::string escape = "\u001b[0m";

    // "\u001b[38;5;231;48;5;81m TEST \u001b[0m"
    // 81 is light blue
    // 244 is slate grey
    if (chr == '-')
    {
        if (highlight)
        {
            return std::string("\u001b[48;5;") + highlight_color + "m" + "-" + escape;
        }
        return std::string("-");
    }

    return (white ? white_color : black_color) +
           (highlight ? highlight_code + highlight_color : "") + "m" +
           chr + escape;
}

std::string piece_to_unicode_char(piece_t piece)
{
    std::string unicode_str = UNICODE_PIECE_CHAR_MAP[piece];
    bool white = is_white_piece(piece);

    if (!unicode_str.compare("-"))
    {
        return std::string("-");
    }

    return (white ? std::string("\u001b[38;5;231m") : std::string("\u001b[38;5;94m")) + unicode_str + std::string("\u001b[0m");
}

char old_piece_to_char(piece_t piece)
{
    bool white = is_white_piece(piece);
    char piece_char;
    switch (piece & PIECE_MASK)
    {
    case PAWN:
        piece_char = 'P';
        break;
    case ROOK:
        piece_char = 'R';
        break;
    case KNIGHT:
        piece_char = 'N';
        break;
    case BISHOP:
        piece_char = 'B';
        break;
    case QUEEN:
        piece_char = 'Q';
        break;
    case KING:
        piece_char = 'K';
        break;
    default:
        return '-';
    }
    return piece_char + (white ? 0x0 : 0x20);
}