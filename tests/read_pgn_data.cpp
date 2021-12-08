#include "catch.hpp"
#include "representation/position.hpp"
#include "process_pgn/read_pgn_data.hpp"
#include "representation/fen.hpp"
#include <filesystem>

TEST_CASE("pgn processor creates appropriate tablebase", "pgnProcessor")
{
    const fs::path tablebase_test_dir = fs::path("/tmp") / program_start_timestamp;
    const fs::path pgn_test_database_path = fs::path(TEST_ROOT_DIR) /
                                            "database" / "pgn" / "test_01";
    const std::string tablebase_name = "test_tb";

    PgnProcessor pgnProcessor(tablebase_test_dir / tablebase_name, pgn_test_database_path);
    pgnProcessor.process_pgn_files();

    auto position = starting_position();
    z_hash_t hash_01 = zobrist_hash(position.get());
    position->advance_position(m(E2_SQ, E4_SQ));
    z_hash_t hash_02 = zobrist_hash(position.get());
    position->advance_position(m(B7_SQ, B6_SQ));
    z_hash_t hash_03 = zobrist_hash(position.get());

    auto position2 = starting_position();
    position2->advance_position(m(E2_SQ, E4_SQ));
    position2->advance_position(m(C7_SQ, C5_SQ));
    z_hash_t hash_04 = zobrist_hash(position2.get());
    position2->advance_position(m(G1_SQ, F3_SQ));
    z_hash_t hash_05 = zobrist_hash(position2.get());

    REQUIRE(((*pgnProcessor.get_tablebase())[hash_01] != NULL));
    REQUIRE(((*pgnProcessor.get_tablebase())[hash_02] != NULL));
    REQUIRE(((*pgnProcessor.get_tablebase())[hash_03] == NULL));
    REQUIRE(((*pgnProcessor.get_tablebase())[hash_04] != NULL));
    REQUIRE(((*pgnProcessor.get_tablebase())[hash_05] != NULL));

    REQUIRE(((*pgnProcessor.get_tablebase())[hash_01]->size() == 1));
    REQUIRE(((*pgnProcessor.get_tablebase())[hash_05]->size() == 2));
}

TEST_CASE("tablebase read from disk is equivalent to created tablebase", "pgnProcessor")
{
    const fs::path tablebase_test_dir = fs::path("/tmp") / program_start_timestamp;
    const fs::path pgn_test_database_path = fs::path(TEST_ROOT_DIR) /
                                            "database" / "pgn" / "test_01";
    const std::string tablebase_name = "test_tb";

    PgnProcessor pgnProcessor(tablebase_test_dir / tablebase_name, pgn_test_database_path);
    pgnProcessor.process_pgn_files();
    pgnProcessor.serialize_all();

    Tablebase tablebase(tablebase_test_dir / tablebase_name);

    REQUIRE((tablebase == (*pgnProcessor.get_tablebase().get())));
}

TEST_CASE("order in which games appear in pgn file doesn't affect binary contents of serialized tablebase", "pgnProcessor")
{
    const fs::path tablebase_test_dir = fs::path("/tmp") / program_start_timestamp;
    const fs::path pgn_test_database_path_a = fs::path(TEST_ROOT_DIR) /
                                              "database" / "pgn" / "test_02a";
    const fs::path pgn_test_database_path_b = fs::path(TEST_ROOT_DIR) /
                                              "database" / "pgn" / "test_02b";
    const std::string tablebase_name_a = "test_tb_02a";
    const std::string tablebase_name_b = "test_tb_02b";

    PgnProcessor pgnProcessor_a(tablebase_test_dir / tablebase_name_a, pgn_test_database_path_a);
    pgnProcessor_a.process_pgn_files();
    pgnProcessor_a.serialize_all();

    PgnProcessor pgnProcessor_b(tablebase_test_dir / tablebase_name_b, pgn_test_database_path_b);
    pgnProcessor_b.process_pgn_files();
    pgnProcessor_b.serialize_all();

    std::streampos size_a;
    std::streampos size_b;
    char *data_a;
    char *data_b;
    for (size_t shard = 0; shard < Tablebase::get_shard_count(); shard++)
    {
        std::stringstream file_suffix;
        file_suffix << std::setw(3) << std::setfill('0') << std::to_string(shard) << ".tb";

        std::ifstream infile_a(tablebase_test_dir / tablebase_name_a / file_suffix.str(),
                               std::ios::binary | std::ios::ate);

        size_a = infile_a.tellg();
        data_a = new char[size_a];
        infile_a.seekg(0, std::ios::beg);
        infile_a.read(data_a, size_a);
        infile_a.close();

        std::ifstream infile_b(tablebase_test_dir / tablebase_name_a / file_suffix.str(),
                               std::ios::binary | std::ios::ate);

        size_b = infile_b.tellg();
        data_b = new char[size_b];
        infile_b.seekg(0, std::ios::beg);
        infile_b.read(data_b, size_b);
        infile_b.close();

        REQUIRE(size_a == size_b);
        REQUIRE(strcmp(data_a, data_b) == 0);
    }
}

TEST_CASE("process_pgn_disambiguates_pgn_moves", "pgnProcessor")
{
    const fs::path tablebase_test_dir = fs::path("/tmp") / program_start_timestamp;
    const fs::path pgn_test_database_path = fs::path(TEST_ROOT_DIR) /
                                            "database" / "pgn" / "test_05";
    const std::string tablebase_name = "test_tb";

    PgnProcessor pgnProcessor(tablebase_test_dir / tablebase_name, pgn_test_database_path);
    pgnProcessor.set_max_plies(50);
    pgnProcessor.process_pgn_files();

    auto tablebase_ptr = pgnProcessor.get_tablebase();

    const std::string fen = "r3k2r/p2n1p1p/1pb3p1/2N1p2Q/1q1p4/6N1/PPPN1PPP/R1B1KB1R w KQkq - 2 15";
    auto pos = fen_to_position(fen);
    auto hash = zobrist_hash(pos.get());

    REQUIRE(tablebase_ptr->position_exists(hash));
    auto moves = (*tablebase_ptr)[hash];
    auto move_edge = moves->at(m(C5_SQ, E4_SQ));

    pos->advance_position(m(C5_SQ, E4_SQ));
    auto dest_hash = zobrist_hash(pos.get());
    REQUIRE(move_edge.m_dest_hash == dest_hash);
}

TEST_CASE("process larger amount of pgn files successfully", "pgnProcessor")
{
    const fs::path tablebase_test_dir = fs::path("/tmp") / program_start_timestamp;
    const fs::path pgn_test_database_path = fs::path(TEST_ROOT_DIR) /
                                            "database" / "pgn" / "test_04";
    const std::string tablebase_name = "test_tb";

    PgnProcessor pgnProcessor(tablebase_test_dir / tablebase_name, pgn_test_database_path);
    pgnProcessor.process_pgn_files();
    pgnProcessor.serialize_all();

    REQUIRE(pgnProcessor.get_tablebase()->total_size() == 54896);
}

// TEST_CASE("pgn processor recognizes illegal move in pgn file", "pgnProcessor")
// {

// TODO exceptions are thrown from threads so you would have to write some extra code
// to catch it in the main thread, so that you can use REQUIRE_THROWS

// const fs::path tablebase_test_dir = fs::path("/tmp") / program_start_timestamp;
// const fs::path pgn_test_database_path = fs::path(TEST_ROOT_DIR) /
//                                         "database" / "pgn" / "test_03";
// const std::string tablebase_name = "test_tb";

// PgnProcessor pgnProcessor(tablebase_test_dir / tablebase_name, pgn_test_database_path);
// // REQUIRE_THROWS(pgnProcessor.process_pgn_files());
// try
// {
//     pgnProcessor.process_pgn_files();
// }
// catch (std::exception &ex)
// {

//     REQUIRE(true);
// }
// }
