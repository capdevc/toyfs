#pragma once
#include <sys/types.h>
#include <memory>
#include <array>
#include <list>
#include <vector>
#include <string>
#include <fstream>

enum Mode { r, w, rw };

enum EntryType { file, dir };

class ToyFS {
  class Inode {
    public:

    uint size;
    uint block_size;
    std::array<uint, 50>d_blocks;
    std::unique_ptr < std::array < std::array<uint, 50>, 50 >> i_blocks;
  };

  class DirEntry {
    public:

    EntryType type;
    std::string name;
    std::weak_ptr<DirEntry>parent;
    std::shared_ptr<Inode>inode;
    std::list < std::shared_ptr < DirEntry >> subdirs;
    DirEntry(const std::string              name,
             const std::weak_ptr<DirEntry>& parent);
    DirEntry(const std::string              name,
             const std::weak_ptr<DirEntry>& parent,
             const std::shared_ptr<Inode> & inode);

    // move creation out to toyfs
  };

  class FreeNode {
    public:

    uint num_blocks;
    uint pos;
    FreeNode(uint num_blocks, uint pos)
      : num_blocks(num_blocks),
      pos(pos) {}
  };

  const std::string filename;
  std::fstream disk_file;
  const uint fs_size;
  const uint block_size;
  const uint num_blocks;

  // DirEntry root;
  std::list<FreeNode>free_nodes;

  void init_disk(const std::string& filename);

  public:

  ToyFS(const std::string& filename, const uint fs_size, const uint block_size);
  ~ToyFS();
};
