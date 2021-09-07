#pragma once

#include "representation/position.hpp"
#include <cstdint>
#include <vector>

bool white_attacks_diagonally(uint8_t piece);
bool black_attacks_diagonally(uint8_t piece);
bool white_attacks_files_ranks(uint8_t piece);
bool black_attacks_files_ranks(uint8_t piece);

bool is_w_pawn(uint8_t piece);
bool is_b_pawn(uint8_t piece);
bool is_w_bishop(uint8_t piece);
bool is_w_rook(uint8_t piece);
bool is_w_queen(uint8_t piece);
bool is_b_bishop(uint8_t piece);
bool is_b_rook(uint8_t piece);
bool is_b_queen(uint8_t piece);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_pawn_moves(std::shared_ptr<Position> position,
                                uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_king_moves(std::shared_ptr<Position> position,
                                uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_castling_king_moves(std::shared_ptr<Position> position,
                                         uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_knight_moves(std::shared_ptr<Position> position,
                                  uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_rook_moves(std::shared_ptr<Position> position,
                                uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_bishop_moves(std::shared_ptr<Position> position,
                                  uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_queen_moves(std::shared_ptr<Position> position,
                                 uint8_t square);

template <Color C>
std::vector<uint8_t>
generate_pseudolegal_piece_moves(std::shared_ptr<Position> position,
                                 uint8_t square);

std::vector<uint8_t>
generate_pseudolegal_piece_moves(std::shared_ptr<Position> position,
                                 uint8_t square);
