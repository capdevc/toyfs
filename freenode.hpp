#ifndef _FREENODE_H_
#define _FREENODE_H_

#include <sys/types.h>

class FreeNode {
 public:
  uint num_blocks;
  uint pos;
  FreeNode(uint num_blocks, uint pos)
      : num_blocks(num_blocks),
        pos(pos) {}
};

#endif /* _FREENODE_H_ */

