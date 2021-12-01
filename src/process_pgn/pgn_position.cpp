/*
    Contains position-related functions that are used during the parsing of PGN files.
    Some of them are implemented naively, so I'm not sure if I'll be using them
    during more important things such as move generation. I've placed them separately here for now.
*/

#include "representation/position.hpp"
#include "representation/offsets.hpp"
#include "representation/notation.hpp"
#include "tablebase/tablebase.hpp"
#include <thread>

uint32_t Position::castling_move(std::smatch &matches, bool white)
{
    m_whites_turn = white;

    std::string whichever_castle = matches[1];
    std::string queenside_castle = matches[2];
    std::string kingside_castle = matches[3];
    std::string check_or_mate = matches[4];

    uint8_t src_square = INVALID_SQUARE;
    uint8_t dest_square = INVALID_SQUARE;
    bool short_castle = false;

    src_square = white ? W_KING_SQUARE : B_KING_SQUARE;

    if (kingside_castle.size())
    {
        dest_square = white ? W_KING_SHORT_CASTLE_SQUARE : B_KING_SHORT_CASTLE_SQUARE;
        short_castle = true;
    }
    else if (queenside_castle.size())
    {
        dest_square = white ? W_KING_LONG_CASTLE_SQUARE : B_KING_LONG_CASTLE_SQUARE;
    }
    else
    {
        assert(false);
    }

    perform_castle(white, short_castle);
    // print_with_borders_highlight_squares(src_square, dest_square);

    return generate_move_key(src_square, dest_square, 0);
}

void Position::perform_castle(bool white, bool short_castle)
{
    assert(m_mailbox[white ? W_KING_SQUARE : B_KING_SQUARE] == white ? W_KING : B_KING);
    m_mailbox[white ? W_KING_SQUARE : B_KING_SQUARE] = 0;
    if (white)
    {
        if (short_castle)
        {
            m_mailbox[W_KING_ROOK_SQUARE] = 0;

            assert(m_mailbox[W_KING_SHORT_CASTLE_SQUARE] == 0);
            assert(m_mailbox[W_ROOK_SHORT_CASTLE_SQUARE] == 0);

            m_mailbox[W_KING_SHORT_CASTLE_SQUARE] = W_KING;
            m_mailbox[W_ROOK_SHORT_CASTLE_SQUARE] = W_ROOK;
        }
        else
        {
            m_mailbox[W_QUEEN_ROOK_SQUARE] = 0;

            assert(m_mailbox[W_KING_LONG_CASTLE_SQUARE] == 0);
            assert(m_mailbox[W_ROOK_LONG_CASTLE_SQUARE] == 0);

            m_mailbox[W_KING_LONG_CASTLE_SQUARE] = W_KING;
            m_mailbox[W_ROOK_LONG_CASTLE_SQUARE] = W_ROOK;
        }
    }
    else
    {
        if (short_castle)
        {
            m_mailbox[B_KING_ROOK_SQUARE] = 0;

            assert(m_mailbox[B_KING_SHORT_CASTLE_SQUARE] == 0);
            assert(m_mailbox[B_ROOK_SHORT_CASTLE_SQUARE] == 0);

            m_mailbox[B_KING_SHORT_CASTLE_SQUARE] = B_KING;
            m_mailbox[B_ROOK_SHORT_CASTLE_SQUARE] = B_ROOK;
        }
        else
        {
            m_mailbox[B_QUEEN_ROOK_SQUARE] = 0;

            assert(m_mailbox[B_KING_LONG_CASTLE_SQUARE] == 0);
            assert(m_mailbox[B_ROOK_LONG_CASTLE_SQUARE] == 0);

            m_mailbox[B_KING_LONG_CASTLE_SQUARE] = B_KING;
            m_mailbox[B_ROOK_LONG_CASTLE_SQUARE] = B_ROOK;
        }
    }
    m_en_passant_square = INVALID_SQUARE;
}

// Non-trivial because this
// involves a minimal amount of move logic in order to calculate correctly.
// For instance assume you have an empty board except for two white rooks on
// e4 and d4. Let's say the move played was Ra4. It is easy for us to see that
// only the rook on the d file could have gone to a4, because the e file rook
// is blocked by the d file rook. However, to program this correctly, I would
// need to scan the rank, file, or diagonal (depending on the piece) to
// determine this. To make matters worse, if two pieces of the same type could
// potentially move to the same square, but moving one would be illegal due to
// discovered check, then the move notation does not include the source
// file/rank.
uint32_t Position::non_castling_move(
    char piece_char, char src_file, char src_rank, char capture,
    char dest_file, char dest_rank, char promotion_piece,
    char check_or_mate)
{
    uint8_t src_square = 0x7f;
    uint8_t dest_square = 0x7f;
    uint8_t new_en_passant_square = INVALID_SQUARE;
    Color color = m_whites_turn ? Color::WHITE : Color::BLACK;

    // destination file and rank should be present in every non-castling move
    assert(dest_file && dest_rank);
    dest_square = an_square_to_index(dest_file, dest_rank);

    if (bool is_pawn_move = !piece_char; is_pawn_move)
    {
        assert(!src_rank);

        // first 8 bits are EP square, second 8 bits are src_square
        uint16_t mixture = get_src_square_pawn_move(capture, src_file, dest_square, dest_rank);
        src_square = mixture & 0xff;
        new_en_passant_square = mixture >> 8;
    }

    // There is only one king per side so this should be simple
    // There should never be ambiguity.

    else if (piece_char == KING_CHAR)
    {
        src_square = find_king(m_whites_turn);
        assert(is_valid_square(src_square));

        // assert that the square that we found the king at, is one square away from the square he supposedly moved to.
        bool found_orig = false;
        for (auto it = directions_vector.begin(); it != directions_vector.end(); it++)
        {
            uint8_t check = dest_square + direction_offset(*it);
            if (check == src_square)
            {
                found_orig = true;
                break;
            }
        }
        assert(found_orig);
    }

    // Very rare for both of src_file and src_rank to be present. Only for some
    // moves like Qc1c2 or Nf3d4 where there are multiple pieces of the same type
    // that could go to the same square, and both src file and src rank are necessary
    // for the move not to be ambigous. An example of this is move 54 in the custom
    // lichess game in the test pgn data.
    else if (src_file && src_rank)
    {
        src_square = an_square_to_index(src_file, src_rank);
    }

    // else if (is_minor_major_piece(piece_char))
    else if (piece_char == KNIGHT_CHAR || piece_char == BISHOP_CHAR || piece_char == ROOK_CHAR || piece_char == QUEEN_CHAR)
    {
        src_square = get_src_square_minmaj_piece_move(piece_char, src_file, src_rank, dest_square, new_en_passant_square);
    }

    assert_correct_player_turn(src_square, dest_square);

    if (is_invalid_square(src_square))
    {
        std::cout << "Impl incomplete for move. " << std::endl;
        assert(false);
    }
    else
    {
        advance_position(src_square, dest_square, promotion_piece);
        // print_with_borders_highlight_squares(src_square, dest_square);
    }

    return generate_move_key(src_square, dest_square, promotion_piece);
}

/*
  Returns the square with the (white ? white : black) king. Returns 127 otherwise.
*/
square_t Position::find_king(bool white)
{
    uint8_t target = white ? W_KING : B_KING;
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            if (uint8_t square = (16 * rank) + file; m_mailbox[square] == target)
            {
                return square;
            }
        }
    }
    return INVALID_SQUARE;
}

// if it is player X's turn to move, returns true if their king is currently in check.
// TODO LASTLEFTOFF look at pgnposition notes
bool Position::is_king_in_check()
{
    Color enemy_color = m_whites_turn ? Color::BLACK : Color::WHITE;

    // uint8_t king_square = find_king();
    return false;
}

// check that whosever turn it is, isn't attacking the king
bool Position::legal_position()
{
    Color attacker_color = m_whites_turn ? Color::WHITE : Color::BLACK;

    // If it is white's turn to move, make sure white isnt already attacking black king
    uint8_t king_square = find_king(!m_whites_turn);

    // look for pawns attacking king
    uint8_t target = m_whites_turn ? W_PAWN : B_PAWN;
    uint8_t candidate = PREV_FILE(BACKWARD_RANK(attacker_color, king_square));
    if (is_valid_square(candidate) && m_mailbox[candidate] == target)
    {
        return false;
    }
    candidate = NEXT_FILE(BACKWARD_RANK(attacker_color, king_square));
    if (is_valid_square(candidate) && m_mailbox[candidate] == target)
    {
        return false;
    }

    // look for knights attacking king
    std::vector<uint8_t> knights;
    target = m_whites_turn ? W_KNIGHT : B_KNIGHT;
    for (auto it = knight_move_offsets.begin(); it != knight_move_offsets.end(); it++)
    {
        candidate = *it + king_square;
        if (is_invalid_square(candidate))
        {
            continue;
        }
        if (m_mailbox[candidate] == target)
        {
            return false;
        }
    }

    if (!check_diagonals(this, king_square, m_whites_turn).empty())
    {
        return false;
    }

    //look for bishops/queens attacking king on diagonals
    if (!check_files_ranks(this, king_square, m_whites_turn).empty())
    {
        return false;
    }

    // look for kings next to each other. it doesnt matter whose turn it is when this happens, its always illegal.
    target = m_whites_turn ? W_KING : B_KING;
    for (auto it = directions_vector.begin(); it != directions_vector.end(); it++)
    {
        candidate = king_square + direction_offset(*it);
        if (is_valid_square(candidate) && m_mailbox[candidate] == target)
        {
            return false;
        }
    }

    return true;
}

void _throw(bool b, const char *assertion_description)
{
    if (!b)
        throw std::invalid_argument(assertion_description);
}

uint16_t Position::get_src_square_pawn_move(char capture, char src_file, square_t dest_square, uint8_t dest_rank)
{
    uint8_t src_rank = 0;
    uint8_t src_square = 0;
    // there should never be a src_rank when it's a pawn move
    uint8_t target = m_whites_turn ? W_PAWN : B_PAWN;
    uint8_t new_en_passant_square = 0;

    if (capture)
    {
        // if we are capturing, the src_file should be present
        _throw(src_file, "src_file should be non-zero");
        src_rank = m_whites_turn ? dest_rank - 1 : dest_rank + 1;
        src_square = an_square_to_index(src_file, src_rank);
        _throw(m_mailbox[src_square] == target, "src_square should contain the pawn that is moving");
        _throw(is_piece(m_mailbox[dest_square]) || m_en_passant_square == dest_square, "either we are capturing a piece or pawn is moving to en passant square");
    }
    else
    {
        _throw(m_mailbox[dest_square] == 0, "if the pawn isn't capturing, the square it is moving to should be empty");
        Direction direction = m_whites_turn ? Direction::DOWN : Direction::UP;
        uint8_t candidate_square = STEP_DIRECTION(direction, dest_square);
        uint8_t candidate_en_passant_square = candidate_square;
        if (m_mailbox[candidate_square] == target)
        {
            src_square = candidate_square;
        }
        else
        {
            // If the pawn didn't come from the square we just checked, that
            // square must be empty
            _throw(m_mailbox[candidate_square] == 0, "if pawn is moving two squares, the square it skips needs to be empty");
            candidate_square = STEP_DIRECTION(direction, candidate_square);
            _throw(m_mailbox[candidate_square] == target, "src_square should have the moving pawn");
            src_square = candidate_square;
            new_en_passant_square = candidate_en_passant_square;
        }
    }

    // first 8 bits are EP square, second 8 bits are src_square
    return (new_en_passant_square << 8) + src_square;
}

uint8_t Position::get_src_square_minmaj_piece_move(char piece_char, uint8_t src_file, uint8_t src_rank, uint8_t dest_square, uint8_t en_passant_square)
{
    std::vector<uint8_t>
        attacking_pieces;

    uint8_t src_square = INVALID_SQUARE;

    switch (piece_char)
    {
    case KNIGHT_CHAR:
    {
        attacking_pieces = find_attacking_knights(this, dest_square, m_whites_turn);
        break;
    }
    case BISHOP_CHAR:
    {
        attacking_pieces = find_attacking_bishops(this, dest_square, m_whites_turn);
        break;
    }
    case ROOK_CHAR:
    {
        attacking_pieces = find_attacking_rooks(this, dest_square, m_whites_turn);
        break;
    }
    case QUEEN_CHAR:
    {
        attacking_pieces = find_attacking_queens(this, dest_square, m_whites_turn);
        break;
    }
    }
    if (attacking_pieces.empty())
    {
        std::cout << "oops" << std::endl;
    }
    assert(!attacking_pieces.empty());

    if (attacking_pieces.size() == 1)
    {
        src_square = attacking_pieces.at(0);
    }
    else
    {
        for (auto it = attacking_pieces.begin(); it != attacking_pieces.end(); it++)
        {
            // if either just the file or just the rank is present, and one of them is equal to the src_file
            // or src_rank from the pgn move OR
            // neither of src_file or src_rank are present
            if (
                ((src_file && index_to_an_file(*it) == src_file) ||
                 (src_rank && src_rank == index_to_an_rank(*it))) ||
                (!src_file && !src_rank))
            {

                auto adjustment = advance_position(*it, dest_square, 0);
                bool is_legal = legal_position();
                undo_adjustment(adjustment);

                if (is_legal)
                {
                    src_square = *it;
                    break;
                }
            }
        }
    }
    if (is_invalid_square(src_square))
    {
        std::cout << "couldn't find_legal " << piece_char << " move." << std::endl;
    }
    return src_square;
}

void Position::assert_correct_player_turn(uint8_t src_square, uint8_t dest_square)
{
    assert(is_valid_square(src_square));
    assert(is_valid_square(dest_square));
    uint8_t moving_piece = m_mailbox[src_square];
    assert(m_whites_turn ? is_white_piece(moving_piece) : is_black_piece(moving_piece));
}
