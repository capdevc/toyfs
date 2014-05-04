#ifndef _TOYFS_H_
#define _TOYFS_H_

#include <fstream>
#include <list>
#include <string>
#include <vector>
#include "inode.hpp"
#include "direntry.hpp"
#include "freenode.hpp"

enum Mode { r, w, rw };

class ToyFS {
  const std::string filename;
  std::fstream disk_file;
  const uint fs_size;
  const uint block_size;
  const uint num_blocks;

  // DirEntry root;
  DirEntry root_dir;
  std::list<FreeNode>free_nodes;

  void init_disk(const std::string& filename);

 public:
  ToyFS(const std::string& filename, const uint fs_size, const uint block_size);
  ~ToyFS();
  void open(std::vector<std::string> args);
  void read(std::vector<std::string> args);
  void write(std::vector<std::string> args);
  void seek(std::vector<std::string> args);
  void close(std::vector<std::string> args);
  void mkdir(std::vector<std::string> args);
  void rmdir(std::vector<std::string> args);
  void cd(std::vector<std::string> args);
  void link(std::vector<std::string> args);
  void unlink(std::vector<std::string> args);
  void stat(std::vector<std::string> args);
  void ls(std::vector<std::string> args);
  void cat(std::vector<std::string> args);
  void cp(std::vector<std::string> args);
  void tree(std::vector<std::string> args);
  void import(std::vector<std::string> args);
  void FS_export(std::vector<std::string> args);
};

#endif /* _TOYFS_H_ */
