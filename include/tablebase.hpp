

struct OpeningTablebase
{
    Game *current_game;
    std::unordered_map<uint64_t, std::vector<move_edge>> opening_tablebase;
};

struct move_edge
{
    uint64_t dest_hash;
    uint32_t times_played;
    // move key is bit-wise concatenation of
    // 0x00 + start_square + end_square + promotion_piece
    uint32_t move_key;
};