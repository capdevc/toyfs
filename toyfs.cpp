#include "toyfs.hpp"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#define ops_at_least(x)                                         \
  if (static_cast<int>(args.size()) < x+1) {                    \
    std::cerr << args[0] << ": missing operand" << std::endl;   \
    return;                                                     \
  }

#define ops_less_than(x)                                        \
  if (static_cast<int>(args.size()) > x+1) {                    \
    std::cerr << args[0] << ": too many operands" << std::endl; \
    return;                                                     \
  }

#define ops_exactly(x)                          \
  ops_at_least(x);                              \
  ops_less_than(x);


ToyFS::ToyFS(const std::string& filename,
             const uint         fs_size,
             const uint         block_size)
    : filename(filename),
      fs_size(fs_size),
      block_size(block_size),
      num_blocks(std::ceil(fs_size / block_size)) {

  init_disk(filename);
  free_nodes.emplace_back(num_blocks, 0);
}

ToyFS::~ToyFS() {
  disk_file.close();
  remove(filename.c_str());
}

void ToyFS::init_disk(const std::string& filename) {
  const std::vector<char>zeroes(num_blocks, 0);

  disk_file.open(filename,
                 std::fstream::in |
                 std::fstream::out |
                 std::fstream::binary |
                 std::fstream::trunc);

  for (uint i = 0; i < num_blocks; ++i) {
    disk_file.write(zeroes.data(), block_size);
  }
}

void ToyFS::open(std::vector<std::string> args) {
  ops_exactly(2);
}

void ToyFS::read(std::vector<std::string> args) {
  ops_exactly(2);
}

void ToyFS::write(std::vector<std::string> args) {
  ops_exactly(2);
}

void ToyFS::seek(std::vector<std::string> args) {
  ops_exactly(2);
}

void ToyFS::close(std::vector<std::string> args) {
  ops_exactly(1);
}

void ToyFS::mkdir(std::vector<std::string> args) {
  ops_at_least(1);
}

void ToyFS::rmdir(std::vector<std::string> args) {
  ops_at_least(1);
}

void ToyFS::cd(std::vector<std::string> args) {
  ops_exactly(1);
}


void ToyFS::link(std::vector<std::string> args) {
  ops_exactly(2);
}

void ToyFS::unlink(std::vector<std::string> args) {
  ops_exactly(1);
}

void ToyFS::stat(std::vector<std::string> args) {
  ops_at_least(1);
}

void ToyFS::ls(std::vector<std::string> args) {
  ops_exactly(0);
}

void ToyFS::cat(std::vector<std::string> args) {
  ops_at_least(1);
}

void ToyFS::cp(std::vector<std::string> args) {
  ops_exactly(2);
}

void ToyFS::tree(std::vector<std::string> args) {
  ops_exactly(0);
}

void ToyFS::import(std::vector<std::string> args) {
  ops_exactly(2);
}

void ToyFS::FS_export(std::vector<std::string> args) {
  ops_exactly(2);
}
