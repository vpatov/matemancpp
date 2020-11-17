#include "parse_pgn.hpp"

namespace fs = std::filesystem;

void parse_all_pgn_files() {
  parse_pgn_file("/home/vas/repos/matemancpp/database/pgn/Stein.pgn");

  if (true) {
    return;
  }
  std::string path = "/home/vas/repos/matemancpp/database/pgn";
  for (const auto &entry : fs::directory_iterator(path)) {
    parse_pgn_file(entry.path());
  }
}

void parse_pgn_file(std::string file_path) {

  // Check the grammar for some possible issues.

  if (pegtl::analyze<pgn_parser::pgn_file>() != 0) {
    std::cout << "pgn_file grammar failed analysis. " << std::endl;
    exit(1);
  }

  std::vector<pgn_parser::metadata_entry> res;

  pegtl::file_input in(file_path);

  try {
    pegtl::parse<pgn_parser::pgn_file, pgn_parser::action>(in, res);

  } catch (const pegtl::parse_error &e) {
    const auto p = e.positions().front();
    std::cerr << e.what() << std::endl
              << in.line_at(p) << '\n'
              << std::setw(p.column) << '^' << std::endl;
  }
}

void parse_pgn_metadata() {}
void parse_pgn_moves() {}
