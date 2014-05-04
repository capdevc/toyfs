#include "toyfs.hpp"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using std::cerr;
using std::endl;
using std::fstream;
using std::make_shared;
using std::string;
using std::vector;

#define ops_at_least(x)                                         \
  if (static_cast<int>(args.size()) < x+1) {                    \
    cerr << args[0] << ": missing operand" << endl;             \
    return;                                                     \
  }

#define ops_less_than(x)                                        \
  if (static_cast<int>(args.size()) > x+1) {                    \
    cerr << args[0] << ": too many operands" << endl;           \
    return;                                                     \
  }

#define ops_exactly(x)                          \
  ops_at_least(x);                              \
  ops_less_than(x);


ToyFS::ToyFS(const string& filename,
             const uint         fs_size,
             const uint         block_size)
    : filename(filename),
      fs_size(fs_size),
      block_size(block_size),
      num_blocks(ceil(fs_size / block_size)) {

  root_dir = make_shared<DirEntry>(DirEntry("root", root_dir));
  init_disk(filename);
  free_nodes.emplace_back(num_blocks, 0);
}

ToyFS::~ToyFS() {
  disk_file.close();
  remove(filename.c_str());
}

void ToyFS::init_disk(const string& filename) {
  const vector<char>zeroes(num_blocks, 0);

  disk_file.open(filename,
                 fstream::in |
                 fstream::out |
                 fstream::binary |
                 fstream::trunc);

  for (uint i = 0; i < num_blocks; ++i) {
    disk_file.write(zeroes.data(), block_size);
  }
}

void ToyFS::open(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::read(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::write(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::seek(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::close(vector<string> args) {
  ops_exactly(1);
}

void ToyFS::mkdir(vector<string> args) {
  ops_at_least(1);
}

void ToyFS::rmdir(vector<string> args) {
  ops_at_least(1);
}

void ToyFS::cd(vector<string> args) {
  ops_exactly(1);
}


void ToyFS::link(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::unlink(vector<string> args) {
  ops_exactly(1);
}

void ToyFS::stat(vector<string> args) {
  ops_at_least(1);
}

void ToyFS::ls(vector<string> args) {
  ops_exactly(0);
}

void ToyFS::cat(vector<string> args) {
  ops_at_least(1);
}

void ToyFS::cp(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::tree(vector<string> args) {
  ops_exactly(0);
}

void ToyFS::import(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::FS_export(vector<string> args) {
  ops_exactly(2);
}
