#ifndef _INODE_H_
#define _INODE_H_

#include <sys/types.h>
#include <memory>
#include <vector>
#include <string>

class Inode {
 public:
  uint size;
  uint blocks_used;
  std::vector<uint> d_blocks;
  std::unique_ptr<std::vector<std::vector<uint>>> i_blocks;

  Inode();
};

#endif /* _INODE_H_ */
