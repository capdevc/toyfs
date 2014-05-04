#include "direntry.hpp"
#include <algorithm>
#include <sstream>
#include <vector>
#

using std::find_if;
using std::istringstream;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;

DirEntry::DirEntry(const string name,
                   const weak_ptr<DirEntry> parent)
    : type(dir), name(name), parent(parent) {}

DirEntry::DirEntry(const string name,
                   const weak_ptr<DirEntry> parent,
                   const shared_ptr<Inode> & inode)
    : type(file), name(name), parent(parent), inode(inode) {}

shared_ptr<DirEntry> DirEntry::find_child(const string name) const {
  auto named = [&] (const shared_ptr<DirEntry> de) {return de->name == name;};
  auto it = find_if(begin(contents), end(contents), named);
  if (it == end(contents)) {
    return nullptr;
  }
  return *it;
}

shared_ptr<DirEntry> DirEntry::add_dir(const string name) {
  auto new_dir = make_shared<DirEntry>(DirEntry(name,
                                                shared_from_this()));
  contents.push_back(new_dir);
  return new_dir;
}

shared_ptr<DirEntry> DirEntry::add_file(const string name) {
  auto new_file = make_shared<DirEntry>(DirEntry(name,
                                                 shared_from_this(),
                                                 make_shared<Inode>()));
  contents.push_back(new_file);
  return new_file;
}
