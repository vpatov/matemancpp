#include "process_pgn/pgn_game.hpp"
#include "process_pgn/read_pgn_data.hpp"
#include "tablebase/tablebase.hpp"

bool PgnGame::read_metadata_line(std::string &line)
{
    std::smatch matches;
    if (std::regex_match(line, matches, std::regex(metadata_line_regex)))
    {
        metadata_entry entry;
        entry.key = std::string(matches[1]);
        entry.value = std::string(matches[2]);
        m_metadata.push_back(entry);
        return true;
    }
    return false;
}

void PgnGame::process_player_move(std::string player_move, bool whites_turn, Tablebase *masterTablebase)
{
    uint32_t move_key;
    boost::algorithm::trim(player_move);

    if (player_move.size() == 0)
    {
        return;
    }

    m_position.m_whites_turn = whites_turn;

    z_hash_t zhash1 = zobrist_hash(&m_position);

    std::smatch matches;
    if (std::regex_match(player_move, matches,
                         std::regex(non_castling_move_regex)))
    {
        char piece_char = getc(1, matches);
        char src_file = getc(2, matches);
        char src_rank = getc(3, matches);
        char capture = getc(4, matches);
        char dest_file = getc(5, matches);
        char dest_rank = getc(6, matches);
        std::string promotion = matches[7];
        char promotion_piece = 0;
        char check_or_mate = getc(8, matches);

        // Get the promotion piece
        if (promotion.size())
        {
            promotion_piece = char_to_piece(promotion.at(1));
            // piece should always be uppercase because pieces are uppercase in PGN.
            assert(promotion_piece < PIECE_MASK);

            if (!whites_turn)
            {
                promotion_piece |= BLACK_PIECE_MASK;
            }
        }

        move_key = m_position.non_castling_move(
            piece_char, src_file, src_rank,
            capture, dest_file, dest_rank,
            promotion_piece, check_or_mate);
    }
    else if (std::regex_match(player_move, matches,
                              std::regex(castling_move_regex)))
    {
        move_key = m_position.castling_move(matches, whites_turn);
    }
    else
    {
        // expected regex to match a move.
        assert(false);
    }
    // push the parsed move key to the move list
    m_move_list.push_back(move_key);
    m_position.m_plies++;

    m_position.m_whites_turn = !whites_turn;
    z_hash_t zhash2 = zobrist_hash(&m_position);

    // master tablebase update here
    masterTablebase->update(zhash1, zhash2, move_key, std::string(player_move));
}

bool PgnGame::read_game_move_line(std::string &line, Tablebase *masterTablebase)
{

    bool is_game_line = false;
    std::smatch matches;
    while (std::regex_search(line, matches, game_line_regex))
    {
        is_game_line = true;

        // stop processing moves after ply 15 such that the tablebases arent too large.
        if (m_position.m_plies < 15)
        {
            process_player_move(matches[1], true, masterTablebase);
            process_player_move(matches[2], false, masterTablebase);
        }

        if (matches[3].length() > 0)
        {
            process_result(matches[3]);
        }
        line = matches.suffix().str();
    }

    return is_game_line;
}

void PgnGame::process_result(std::string resultstr)
{
    m_result = std::move(resultstr);
    m_finishedReading = true;
}

void PgnGame::printGameSummary()
{

    using namespace std;
    cout
        << ColorCode::end
        << left << setw(32) << m_white_player_name
        << left << setw(32) << m_black_player_name
        << ColorCode::green
        << left << setw(30) << m_event
        << ColorCode::yellow
        << left << setw(14) << m_result
        << ColorCode::purple
        << left << setw(10) << m_move_list.size()
        << ColorCode::white
        << left << setw(10) << m_whiteElo
        << left << setw(10) << m_blackElo
        << ColorCode::end
        << std::endl;
}

// Reads the elo, and other metadata.
void PgnGame::populateMetadata()
{
    int elo;

    // Iterate through metadata key-value pairs
    for (auto it = m_metadata.begin(); it != m_metadata.end(); it++)
    {
        if (it->value.length() < 2)
        {
            continue;
        }
        if ((it->key).compare("WhiteElo") == 0)
        {
            elo = std::stoi(it->value);
            if (elo >= 0)
            {
                m_whiteElo = elo;
            }
        }
        else if ((it->key).compare("BlackElo") == 0)
        {
            elo = std::stoi(it->value);
            if (elo >= 0)
            {
                m_blackElo = elo;
            }
        }
        else if ((it->key).compare("Event") == 0)
        {
            m_event = std::string(it->value);
        }
        else if ((it->key).compare("White") == 0)
        {
            m_white_player_name = std::string(it->value);
        }
        else if ((it->key).compare("Black") == 0)
        {
            m_black_player_name = std::string(it->value);
        }
    }
    m_eloOverThreshold =
        m_whiteElo >= ELO_THRESHOLD && m_blackElo >= ELO_THRESHOLD;
}
