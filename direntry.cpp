#include "direntry.hpp"
#include <memory>
#include <string>
#include <sstream>
#include <vector>

std::vector<std::string> parse_path(std::string path_str) {
  std::istringstream is(path_str);
  std::string token;
  std::vector<std::string> tokens;

  while (std::getline(is, token, '/')) {
    tokens.push_back(token);
  }
  return tokens;
}

DirEntry::DirEntry(const std::string name,
                   const DirEntry &parent)
    : type(dir), name(name), parent(parent) {}

DirEntry::DirEntry(const std::string name,
                   const DirEntry &parent,
                   const std::shared_ptr<Inode> & inode)
    : type(file), name(name), parent(parent), inode(inode) {}
