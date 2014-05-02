#include "toyfs.hpp"
#include <cmath>
#include <stdio.h>

ToyFS::DirEntry::DirEntry(const std::string              name,
                          const std::weak_ptr<DirEntry>& parent)
  : name(name), parent(parent) {}

ToyFS::DirEntry::DirEntry(const std::string              name,
                          const std::weak_ptr<DirEntry>& parent,
                          const std::shared_ptr<Inode> & inode)
  : name(name), parent(parent), inode(inode) {}

void ToyFS::init_disk(const std::string& filename)
{
  const std::vector<char>zeroes(num_blocks, 0);

  disk_file.open(filename,
                   std::fstream::in |
                   std::fstream::out |
                   std::fstream::binary |
                   std::fstream::trunc);

  for (uint i = 0; i < num_blocks; ++i) {
    disk_file.write(zeroes.data(), block_size);
  }
}

ToyFS::ToyFS(const std::string& filename,
             const uint         fs_size,
             const uint         block_size)
  : filename(filename),
  fs_size(fs_size),
  block_size(block_size),
  num_blocks(std::ceil(fs_size / block_size))
{
  init_disk(filename);
  free_nodes.emplace_back(num_blocks, 0);
}

ToyFS::~ToyFS()
{
  disk_file.close();
  remove(filename.c_str());
}

void ToyFS::open(std::vector<std::string>) {}
void ToyFS::read(std::vector<std::string>) {}
void ToyFS::write(std::vector<std::string>) {}
void ToyFS::seek(std::vector<std::string>) {}
void ToyFS::close(std::vector<std::string>) {}
void ToyFS::mkdir(std::vector<std::string>) {}
void ToyFS::rmdir(std::vector<std::string>) {}
void ToyFS::cd(std::vector<std::string>) {}
void ToyFS::link(std::vector<std::string>) {}
void ToyFS::unlink(std::vector<std::string>) {}
void ToyFS::stat(std::vector<std::string>) {}
void ToyFS::ls(std::vector<std::string>) {}
void ToyFS::cat(std::vector<std::string>) {}
void ToyFS::cp(std::vector<std::string>) {}
void ToyFS::tree(std::vector<std::string>) {}
void ToyFS::import(std::vector<std::string>) {}
void ToyFS::FS_export(std::vector<std::string>) {}
