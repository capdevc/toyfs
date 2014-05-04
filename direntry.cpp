#include "direntry.hpp"
#include <memory>
#include <string>
#include <sstream>
#include <vector>

using std::istringstream;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;

vector<string> parse_path(string path_str) {
  istringstream is(path_str);
  string token;
  vector<string> tokens;

  while (getline(is, token, '/')) {
    tokens.push_back(token);
  }
  return tokens;
}

DirEntry::DirEntry(const string name,
                   const shared_ptr<DirEntry> parent)
    : type(dir), name(name), parent(parent) {}

DirEntry::DirEntry(const string name,
                   const shared_ptr<DirEntry> parent,
                   const shared_ptr<Inode> & inode)
    : type(file), name(name), parent(parent), inode(inode) {}

weak_ptr<DirEntry> DirEntry::add_dir(const shared_ptr<DirEntry> &parent,
                                     const string name) {
  shared_ptr<DirEntry> new_dir = make_shared<DirEntry>(DirEntry(name, parent));
  parent->subdirs.push_back(new_dir);
  return new_dir;
}
