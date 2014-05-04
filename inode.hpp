#ifndef _INODE_H_
#define _INODE_H_

#include <sys/types.h>
#include <memory>
#include <vector>

class Inode {
 public:
  uint size;
  uint block_size;
  std::vector<uint>d_blocks;
  std::unique_ptr<std::vector<std::vector<uint>>> i_blocks;
};

#endif /* _INODE_H_ */
