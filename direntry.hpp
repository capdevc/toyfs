#ifndef _DIRENTRY_H_
#define _DIRENTRY_H_

#include <list>
#include <memory>
#include <string>
#include "inode.hpp"

enum EntryType { file, dir };

class DirEntry: public std::enable_shared_from_this<DirEntry> {
 public:
  EntryType type;
  std::string name;
  std::weak_ptr<DirEntry> parent;
  std::shared_ptr<Inode> inode;
  std::list<std::shared_ptr<DirEntry>> subdirs;

  DirEntry(const std::string name,
           const std::weak_ptr<DirEntry> parent);
  DirEntry(const std::string name,
           const std::weak_ptr<DirEntry> parent,
           const std::shared_ptr<Inode> & inode);

  std::shared_ptr<DirEntry> find_child(const std::string name);
  std::weak_ptr<DirEntry> add_dir(const std::string name);
  std::weak_ptr<DirEntry> add_file(const std::string name);
  // move creation out to toyfs
};

#endif /* _DIRENTRY_H_ */
