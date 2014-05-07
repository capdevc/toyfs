#ifndef _INODE_H_
#define _INODE_H_

#include <sys/types.h>
#include <list>
#include <memory>
#include <vector>
#include <string>
#include "freenode.hpp"


class Inode {
 public:
  static uint block_size;
  static std::list<FreeNode> *free_list;
  uint size;
  uint blocks_used;
  std::vector<uint> d_blocks;
  std::unique_ptr<std::vector<std::vector<uint>>> i_blocks;

  Inode();
  ~Inode();
};

#endif /* _INODE_H_ */
