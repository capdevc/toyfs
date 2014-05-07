#include "inode.hpp"
#include <vector>

using std::vector;

Inode::Inode()
    : size(0), blocks_used(0), i_blocks(new vector<vector<uint>>()) {}
