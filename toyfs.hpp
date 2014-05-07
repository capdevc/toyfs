#ifndef _TOYFS_H_
#define _TOYFS_H_

#include <fstream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include "inode.hpp"
#include "direntry.hpp"
#include "freenode.hpp"


class ToyFS {

  enum Mode {R, W, RW};
  struct Descriptor {
    Mode mode;
    uint byte_pos;
    std::weak_ptr<Inode> inode;
    uint fd;
  };
  bool getMode(Mode *mode, std::string mode_s);

  struct PathRet {
    bool invalid_path = false;
    std::string final_name;
    std::shared_ptr<DirEntry> parent_node;
    std::shared_ptr<DirEntry> final_node;
  };

  const std::string filename;
  std::fstream disk_file;
  const uint fs_size;
  const uint block_size;
  const uint direct_blocks;
  const uint num_blocks;

  // DirEntry root;
  std::shared_ptr<DirEntry> root_dir;
  std::shared_ptr<DirEntry> pwd;
  std::list<FreeNode>free_list;
  std::map<uint, Descriptor> open_files;
  uint next_descriptor = 0;

  void init_disk(const std::string& filename);
  std::unique_ptr<PathRet> parse_path(std::string path_str) const;
  bool basic_open(Descriptor *d, std::vector <std::string> args);
  std::unique_ptr<std::string> basic_read(Descriptor &desc, const uint size);
  uint basic_write(Descriptor &desc, const std::string data);

 public:
  ToyFS(const std::string& filename,
        const uint fs_size,
        const uint block_size,
        const uint direct_blocks);
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
  void printwd(std::vector<std::string> args);
  void FS_export(std::vector<std::string> args);
};

#endif /* _TOYFS_H_ */
