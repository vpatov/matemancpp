#include "tablebase/tablebase.hpp"

template <typename T>
static void write(std::fstream *stream, T *data, size_t size)
{
    stream->write(reinterpret_cast<char *>(data), size);
}

void Tablebase::read_from_file(std::string file_path, int shard)
{
    std::streampos size;
    char *data;
    int index = 0;
    std::ifstream infile(file_path, std::ios::binary | std::ios::ate);

    if (infile.is_open())
    {
        size = infile.tellg();

        data = new char[size];
        infile.seekg(0, std::ios::beg);
        infile.read(data, size);
        infile.close();
    }
    else
    {
        std::cerr << ColorCode::red << "Could not open file: " << file_path << ColorCode::end << std::endl;
        assert(false);
    }

    // m_root_hash = *((z_hash_t *)(data + index));
    index += sizeof(z_hash_t);

    uint32_t tablebase_size = *((uint32_t *)(data + index));
    index += sizeof(uint32_t);

    for (int n = 0; n < tablebase_size; n++)
    {
        z_hash_t key_hash = *((z_hash_t *)(data + index));
        index += sizeof(z_hash_t);

        uint32_t move_map_size = *((uint32_t *)(data + index));
        index += sizeof(uint32_t);

        std::shared_ptr<std::unordered_map<MoveKey, MoveEdge>> move_map =
            std::make_shared<std::unordered_map<MoveKey, MoveEdge>>();

        for (int m = 0; m < move_map_size; m++)
        {

            MoveKey move_key = *((uint32_t *)(data + index));
            assert((move_key >> 24) == 0);
            index += sizeof(MoveKey);

            z_hash_t dest_hash = *((z_hash_t *)(data + index));
            index += sizeof(MoveEdge::m_dest_hash);

            char pgn_move[8];
            strncpy(pgn_move, data + index, sizeof(MoveEdge::m_pgn_move));
            index += sizeof(MoveEdge::m_pgn_move);

            uint32_t times_played = *((uint32_t *)(data + index));
            index += sizeof(MoveEdge::m_times_played);

            MoveEdge moveEdge(dest_hash, pgn_move, times_played);

            move_map->insert(std::pair(move_key, moveEdge));
        }

        assert(shard == key_hash % TABLEBASE_SHARD_COUNT);

        shards[shard][key_hash] = move_map;
    }
}

void Tablebase::read_from_directory(fs::path source_directory_path)
{
    int count = 0;
    for (const auto &entry : std::filesystem::directory_iterator(source_directory_path))
    {
        std::string filepath = entry.path().generic_string();
        size_t path_end = filepath.rfind('/');
        size_t extension_start = filepath.rfind(".tb");
        std::string filename = filepath.substr(path_end + 1, extension_start - path_end - 1);
        uint16_t shard = std::stoi(filename);
        read_from_file(filepath, shard);
        count++;
    }
    // TODO make logger global and static, right now it just belongs to the CLI
    // std::cout << ColorCode::green << "Successfully read "
    //           << ColorCode::yellow << count << ColorCode::green << " tablebases"
    //           << ColorCode::end << std::endl;
}

void Tablebase::serialize_tablebase(std::string file_path, int shard)
{
    // serialize_tablebase(file_path_prefix + ".tb", shard);
    std::fstream stream(file_path, std::ios::out | std::ios::binary);

    if (!stream.is_open())
    {
        std::cerr
            << ColorCode::red << "Cannot open filestream to path: " << ColorCode::end << std::endl
            << file_path << std::endl;
        return;
    }

    // 8 bytes (464bits) -> z_hash_t: hash of starting position
    write(&stream, &m_root_hash, sizeof(z_hash_t));

    // 4 bytes (4 bits) -> uint32_t: number of keys in tablebase
    auto position_moves_map = shards[shard];
    uint32_t tablebase_size = position_moves_map.size();
    write(&stream, &tablebase_size, sizeof(uint32_t));

    for (auto node = position_moves_map.begin(); node != position_moves_map.end(); node++)
    {
        z_hash_t key_hash = node->first;
        auto move_map = node->second;

        // 8 bytes (64 bits) -> z_hash_t: key
        write(&stream, &key_hash, sizeof(z_hash_t));

        // // 4 bytes (32 bits) -> uint32_t: length of MoveEdge map (M)
        uint32_t move_map_size = move_map->size();
        write(&stream, &move_map_size, sizeof(uint32_t));

        // MoveEdge serialization (4, 8, 8, 4)
        for (auto it = move_map->begin(); it != move_map->end(); it++)
        {
            MoveKey t_move_key = it->first;
            write(&stream, &(t_move_key), sizeof(MoveKey));
            write(&stream, &(it->second.m_dest_hash), sizeof(MoveEdge::m_dest_hash));
            write(&stream, &(it->second.m_pgn_move), sizeof(MoveEdge::m_pgn_move));
            write(&stream, &(it->second.m_times_played), sizeof(MoveEdge::m_times_played));
        }
    }
    stream.close();
}

void Tablebase::serialize_all(fs::path destination_directory_path)
{
    fs::create_directories(destination_directory_path);

    ThreadPool thread_pool = ThreadPool();
    // Keep an array of functions outside of the loop such that we can use it's address
    // as an argument for the task. If the function is a local variable inside the loop,
    // by the time a thread executes a task the function might be mangled already (since
    // it lives on the stack)
    std::function<void(std::string &)> functions[Tablebase::get_shard_count()];

    for (uint8_t shard = 0; shard < Tablebase::get_shard_count(); shard++)
    {
        std::stringstream file_suffix;
        file_suffix << std::setw(3) << std::setfill('0') << std::to_string(shard) << ".tb";

        functions[shard] =
            std::bind(&Tablebase::serialize_tablebase, this, std::placeholders::_1, shard);

        std::string path = destination_directory_path / file_suffix.str();
        Task task = Task(&functions[shard], path);
        thread_pool.add_task(task);
    }
    thread_pool.join_pool();
}
