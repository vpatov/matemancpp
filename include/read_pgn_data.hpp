#ifndef __READ_PGN_DATA_H__
#define __READ_PGN_DATA_H__

#include "move_generation.hpp"
#include "position.hpp"
#include <boost/algorithm/string.hpp>
#include <codecvt>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>

struct Game;

struct metadata_entry
{
  std::string key;
  std::string value;
};

struct move_edge
{
  uint64_t dest_hash;
  uint32_t times_played;
  // move key is bit-wise concatenation of
  // 0x00 + start_square + end_square + promotion_piece
  uint32_t move_key;
};

const std::string result_regex_str =
    R"((((?:1\/2|1|0)\s*\-\s*(?:1\/2|1|0)\s*$)|\*)?)";
const std::regex game_line_regex(
    R"(\d+\.\s*([\w\-\+\#\=]+)\s([\w\-\+\#\=]+)?\s*)" + result_regex_str);
const std::string metadata_line_regex =
    "^\\s*\\[(\\w+)\\s\"(.*?)\"\\]\\s*$";
const std::string non_castling_move_regex =
    "([RNBKQ])?([a-h])?([1-8])?(x)?([a-h])"
    "([1-8])(=[RNBKQ])?([\\+\\#])?";
const std::string castling_move_regex = "((O-O-O)|(O-O))([\\+\\#])?";

char getc(int i, std::smatch &matches);
uint32_t generate_move_key(uint8_t src_square, uint8_t dest_square, uint8_t promotion_piece);

struct OpeningTablebase
{
  Game *current_game;
  std::unordered_map<uint64_t, std::vector<move_edge>> opening_tablebase;
};

struct Game
{
  std::vector<metadata_entry> metadata;
  Position position;
  int whiteElo;
  int blackElo;
  bool eloOverThreshold;
  bool finishedReading;
  std::string result;
  std::vector<uint32_t> move_list;

  // Adding this constructor causes problems,  because std::vector.emplace_back reallocates?
  // Game()
  // {
  //   populate_starting_position(&position);
  // }

  bool read_metadata_line(std::string &line)
  {
    std::smatch matches;
    if (std::regex_match(line, matches, std::regex(metadata_line_regex)))
    {
      metadata_entry entry;
      entry.key = std::string(matches[1]);
      entry.value = std::string(matches[2]);
      metadata.push_back(entry);
      return true;
    }
    return false;
  }

  void process_player_move(std::string player_move, bool white)
  {
    uint32_t move_key;
    boost::algorithm::trim(player_move);

    if (player_move.size() == 0)
    {
      return;
    }

    std::smatch matches;
    if (std::regex_match(player_move, matches,
                         std::regex(non_castling_move_regex)))
    {
      move_key = non_castling_move(matches, white);
    }
    else if (std::regex_match(player_move, matches,
                              std::regex(castling_move_regex)))
    {
      move_key = castling_move(matches, white);
    }
    else
    {
      assert(false);
    }

    // push the parsed move key to the move list
    std::cout << "Ply: " << position.plies << std::endl;
    move_list.push_back(move_key);

    // TODO remove this!!!
    position.plies++;
  }

  uint32_t castling_move(std::smatch &matches, bool white)
  {
    position.whites_turn = white;

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

    perform_castle(&position, white, short_castle);
    // print_position_with_borders_highlight_squares(&this->position, src_square, dest_square);

    return generate_move_key(src_square, dest_square, 0);
  }

  // This function calculates and returns the move key, which is a concatenation
  // of the source square, destination square, and promotion piece (complete
  // information necessary to understand a move). Non-trivial because this
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
  // TODO: refactor into several smaller functions
  // TODO: consider rewriting this but with using piece lists (would probably be much easier)
  // TODO: make it such that it has no side effects.
  uint32_t non_castling_move(std::smatch &matches, bool white)
  {
    position.whites_turn = white;

    uint8_t src_square = 0x7f;
    uint8_t dest_square = 0x7f;
    uint8_t promotion_piece = 0;
    uint8_t en_passant_square = 0;
    Color color = white ? Color::WHITE : Color::BLACK;

    char piece_char = getc(1, matches);
    char src_file = getc(2, matches);
    char src_rank = getc(3, matches);
    char capture = getc(4, matches);
    char dest_file = getc(5, matches);
    char dest_rank = getc(6, matches);
    std::string promotion = matches[7];
    char check_or_mate = getc(8, matches);

    // Get the promotion piece
    if (promotion.size())
    {
      // std::cout << "PROMOTION: (" << promotion << ")" << std::endl;
      promotion_piece = char_to_piece(promotion.at(1));
      // piece should always be uppercase because pieces are uppercase in PGN.
      assert(promotion_piece < PIECE_MASK);

      if (!white)
      {
        promotion_piece |= BLACK_PIECE_MASK;
      }
    }

    // destination file and rank should be present in every non-castling move
    assert(dest_file && dest_rank);
    dest_square = an_square_to_index(dest_file, dest_rank);

    bool is_pawn_move = !piece_char;
    if (is_pawn_move)
    {
      // there should never be a src_rank when it's a pawn move
      assert(!src_rank);
      uint8_t target = white ? W_PAWN : B_PAWN;

      if (capture)
      {
        // if we are capturing, the src_file should be present
        assert(src_file);
        src_rank = white ? dest_rank - 1 : dest_rank + 1;
        src_square = an_square_to_index(src_file, src_rank);
        assert(position.mailbox[src_square] == target);
        assert(IS_PIECE(position.mailbox[dest_square]) || position.en_passant_square == dest_square);
      }
      else
      {
        Direction direction = white ? Direction::DOWN : Direction::UP;
        uint8_t candidate_square = STEP_DIRECTION(direction, dest_square);
        uint8_t candidate_en_passant_square = candidate_square;
        if (position.mailbox[candidate_square] == target)
        {
          src_square = candidate_square;
        }
        else
        {
          // If the pawn didn't come from the square we just checked, that
          // square must be empty
          assert(position.mailbox[candidate_square] == 0);
          candidate_square = STEP_DIRECTION(direction, candidate_square);
          assert(position.mailbox[candidate_square] == target);
          src_square = candidate_square;
          en_passant_square = candidate_en_passant_square;
        }
      }
    }

    // There is only one king per side so this should be simple
    // There should never be ambiguity.
    else if (piece_char == KING_CHAR)
    {
      src_square = find_king(&position, white);
      assert(IS_VALID_SQUARE(src_square));

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

    else if (piece_char == KNIGHT_CHAR || piece_char == BISHOP_CHAR || piece_char == ROOK_CHAR || piece_char == QUEEN_CHAR)
    {
      std::vector<uint8_t> attacking_pieces;

      switch (piece_char)
      {
      case KNIGHT_CHAR:
      {
        attacking_pieces = find_attacking_knights(&position, dest_square, white);
        break;
      }
      case BISHOP_CHAR:
      {
        attacking_pieces = find_attacking_bishops(&position, dest_square, white);
        break;
      }
      case ROOK_CHAR:
      {
        attacking_pieces = find_attacking_rooks(&position, dest_square, white);
        break;
      }
      case QUEEN_CHAR:
      {
        attacking_pieces = find_attacking_queens(&position, dest_square, white);
        break;
      }
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

            // temporarily assume the move
            adjust_position(&position, *it, dest_square, 0, 0);
            std::cout << "\u001b[31m "
                      << "Testing legality of position:"
                      << "\u001b[0m" << std::endl;

            // ensure the position is legal (king is not in check)
            if (legal_position(&position, white))
            {
              // set the src_square and undo the move.
              src_square = *it;
              adjust_position(&position, dest_square, src_square, 0, 0);
              break;
            }
            else
            {
              std::cout << "\u001b[31m "
                        << "not legal position. adjusting back"
                        << "\u001b[0m" << std::endl;

              // undo the move
              adjust_position(&position, dest_square, *it, 0, 0);
            }
          }
        }
      }
      if (IS_INVALID_SQUARE(src_square))
      {
        std::cout << "couldn't find_legal " << piece_char << " move." << std::endl;
      }
    }

    assert_correct_player_turn(&position, src_square, dest_square);

    if (IS_INVALID_SQUARE(src_square))
    {
      std::cout << "Impl incomplete for move: " << matches[0] << std::endl;
      assert(false);
    }
    else
    {
      adjust_position(&this->position, src_square, dest_square,
                      promotion_piece, en_passant_square);
      // print_position_with_borders_highlight_squares(&this->position, src_square, dest_square);
    }

    return generate_move_key(src_square, dest_square, promotion_piece);
  }

  void assert_correct_player_turn(Position *position, uint8_t src_square, uint8_t dest_square)
  {
    assert(IS_VALID_SQUARE(src_square));
    assert(IS_VALID_SQUARE(dest_square));
    uint8_t moving_piece = position->mailbox[src_square];
    assert(position->whites_turn ? IS_WHITE_PIECE(moving_piece) : IS_BLACK_PIECE(moving_piece));
  }

  void process_result(std::string resultstr)
  {
    result = std::move(resultstr);
    finishedReading = true;
  }

  bool read_game_move_line(std::string &line)
  {

    bool is_game_line = false;
    std::smatch matches;
    while (std::regex_search(line, matches, game_line_regex))
    {
      is_game_line = true;
      std::cout << "=================================" << std::endl;
      if (!metadata.size())
      {
        std::cerr << "No metadata!" << std::endl;
        assert(false);
      }
      std::cout << (metadata.size() ? metadata.at(0).value : "No metadata") << std::endl;
      std::cout << matches[0] << std::endl;

      process_player_move(matches[1], true);
      process_player_move(matches[2], false);
      if (matches[3].length() > 0)
      {
        process_result(matches[3]);
      }
      line = matches.suffix().str();
    }

    return is_game_line;
  }
};

void try_threading();

#define ELO_THRESHOLD 2200

// std::vector<Game> read_pgn_file(std::string file_path);
void read_pgn_file(std::string file_path);
void read_all_pgn_files();

#endif