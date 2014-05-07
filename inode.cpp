#include "inode.hpp"
#include <algorithm>
#include <list>
#include <vector>

using std::list;
using std::shared_ptr;
using std::sort;
using std::vector;

uint Inode::block_size = 0;
list<FreeNode> * Inode::free_list = nullptr;

Inode::Inode()
    : size(0), blocks_used(0), i_blocks(new vector<vector<uint>>()) {}

Inode::~Inode() {
  if (blocks_used == 0) {
    return;
  } else if (blocks_used == 1) {
    free_list->emplace_front(block_size, d_blocks[0]);
  }

  vector<uint> blocks;

  for (uint block : d_blocks) {
    blocks.push_back(block);
  }

  for (auto vec: *i_blocks) {
    for (uint block: vec) {
      blocks.push_back(block);
    }
  }

  sort(begin(blocks), end(blocks));

  uint start = blocks.front();
  uint last = start;
  uint size = block_size;
  blocks.erase(begin(blocks));
  for (uint block : blocks) {
    if (block - last != block_size) {
      free_list->emplace_back(size, start);
      last = start = block;
      size = 0;
    } else {
      last = block;
      size += block_size;
    }
  }
  free_list->emplace_front(size, start);
}

