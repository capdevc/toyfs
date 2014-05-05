#include "direntry.hpp"
#include <algorithm>
#include <sstream>
#include <vector>


using std::find_if;
using std::istringstream;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;

DirEntry::DirEntry() {}

shared_ptr<DirEntry> DirEntry::mk_DirEntry(const string name,
                                           const shared_ptr<DirEntry> parent,
                                           const shared_ptr<Inode> &inode) {
  auto sp = make_shared<DirEntry>(DirEntry());
  if (parent == nullptr) {
    sp->parent = sp;
  } else {
    sp->parent = parent;
  }
  sp->self = sp;
  sp->name = name;
  sp->inode = inode;
  return sp;
}

shared_ptr<DirEntry> DirEntry::find_child(const string name) const {
  // handle . and ..
  if (name == "..") {
    return parent.lock();
  } else if (name == ".") {
    return self.lock();
  }

  // search through contents and return ptr if found, otherwise nullptr
  auto named = [&] (const shared_ptr<DirEntry> de) {return de->name == name;};
  auto it = find_if(begin(contents), end(contents), named);
  if (it == end(contents)) {
    return nullptr;
  }

  return *it;
}

shared_ptr<DirEntry> DirEntry::add_dir(const string name) {
  auto new_dir = mk_DirEntry(name, self.lock());
  new_dir->type = dir;
  contents.push_back(new_dir);
  return new_dir;
}

shared_ptr<DirEntry> DirEntry::add_file(const string name) {
  auto new_file = mk_DirEntry(name, self.lock(), make_shared<Inode>());
  new_file->type = file;
  contents.push_back(new_file);
  return new_file;
}
