#ifndef _DIRENTRY_H_
#define _DIRENTRY_H_

#include <list>
#include <memory>
#include <string>
#include <sys/types.h>
#include "freenode.hpp"
#include "inode.hpp"

enum EntryType { file, dir };

class DirEntry: public std::enable_shared_from_this<DirEntry> {
 public:
  uint block_size;
  EntryType type;
  std::string name;
  std::weak_ptr<DirEntry> parent;
  std::shared_ptr<Inode> inode;
  std::list<std::shared_ptr<DirEntry>> contents;

  DirEntry(const std::string name,
           const std::weak_ptr<DirEntry> parent);
  DirEntry(const std::string name,
           const std::weak_ptr<DirEntry> parent,
           const std::shared_ptr<Inode> & inode);

  std::shared_ptr<DirEntry> find_child(const std::string name) const;
  std::shared_ptr<DirEntry> add_dir(const std::string name);
  std::shared_ptr<DirEntry> add_file(const std::string name);
  // move creation out to toyfs
};

#endif /* _DIRENTRY_H_ */
