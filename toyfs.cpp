#include "toyfs.hpp"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <deque>

using std::cerr;
using std::cout;
using std::endl;
using std::istringstream;
using std::fstream;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;
using std::weak_ptr;
using std::deque;
using std::setw;

#define ops_at_least(x)                                 \
  if (static_cast<int>(args.size()) < x+1) {            \
    cerr << args[0] << ": missing operand" << endl;     \
    return;                                             \
  }

#define ops_less_than(x)                                \
  if (static_cast<int>(args.size()) > x+1) {            \
    cerr << args[0] << ": too many operands" << endl;   \
    return;                                             \
  }

#define ops_exactly(x)                          \
  ops_at_least(x);                              \
  ops_less_than(x);

ToyFS::ToyFS(const string& filename,
             const uint fs_size,
             const uint block_size)
    : filename(filename),
      fs_size(fs_size),
      block_size(block_size),
      num_blocks(ceil(fs_size / block_size)) {

  root_dir = DirEntry::make_de_dir("root", nullptr);
  // start at root dir;
  pwd = root_dir;
  init_disk(filename);
  free_list.emplace_back(num_blocks, 0);
}

ToyFS::~ToyFS() {
  disk_file.close();
  remove(filename.c_str());
}

void ToyFS::init_disk(const string& filename) {
  const vector<char>zeroes(num_blocks, 0);

  disk_file.open(filename,
                 fstream::in |
                 fstream::out |
                 fstream::binary |
                 fstream::trunc);

  for (uint i = 0; i < num_blocks; ++i) {
    disk_file.write(zeroes.data(), block_size);
  }
}

unique_ptr<ToyFS::PathRet> ToyFS::parse_path(string path_str) const {
  unique_ptr<PathRet> ret(new PathRet);

  // check if path is relative or absolute
  ret->final_node = pwd;
  if (path_str[0] =='/') {
    path_str.erase(0);
    ret->final_node = root_dir;
  }
  // initialize data structure
  ret->final_name = ret->final_node->name;
  ret->parent_node = ret->final_node->parent.lock();

  // tokenize the string
  istringstream is(path_str);
  string token;
  vector<string> path_tokens;
  while (getline(is, token, '/')) {
    path_tokens.push_back(token);
  }

  // walk the path updating pointers
  for (auto &node_name : path_tokens) {
    if (ret->parent_node == nullptr) {
      // something other than the last entry was not found
      ret->invalid_path = true;
      return ret;
    }
    ret->parent_node = ret->final_node;
    ret->final_node = ret->final_node->find_child(node_name);
    ret->final_name = node_name;
  }

  return ret;
}

void ToyFS::open(vector<string> args) {
  ops_exactly(2);
  uint mode;
  istringstream(args[2]) >> mode;

  auto path = parse_path(args[1]);
  auto node = path->final_node;
  auto parent = path->parent_node;

  // get the file pointer or create the file
  if (path->invalid_path == true) {
    cerr << "open: error: Invalid path: " << args[1] << endl;
    return;
  } else if (node == root_dir) {
    cerr << "open: error: Cannot open root." << endl;
    return;
  } else if (node == nullptr) {
    if (mode == 1) {
      cout << "open: error: File does not exist." << endl;
      return;
    } else {
      node = parent->add_file(path->final_name);
    }
  }
  if (node->type == dir) {
    cout << "open: error: Cannot open a directory." << endl;
    return;
  }

  // get a descriptor
  uint fd = next_descriptor++;
  open_files[fd] = Descriptor{mode, 0, node->inode};
  cout << fd << endl;
  return;
}

void ToyFS::read(vector<string> args) {
  ops_exactly(2);

  uint fd;
  istringstream(args[1]) >> fd;


}

void ToyFS::write(vector<string> args) {
  ops_at_least(2);
}

void ToyFS::seek(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::close(vector<string> args) {
  ops_exactly(1);
  uint fd;

  if (! (istringstream (args[1]) >> fd)) {
    cerr << "close: error: File descriptor not recognized" << endl;
  } else {
    auto kv = open_files.find(fd);
    if (kv == open_files.end()) {
      cerr << "close: error: File descriptor not open" << endl;
    } else {
      open_files.erase(fd);
      cout << "closed " << fd << endl;
    }
  }
}

void ToyFS::mkdir(vector<string> args) {
  ops_at_least(1);
  /* add each new directory one at a time */
  for (uint i = 1; i < args.size(); i++) {
    auto path = parse_path(args[i]);
    auto node = path->final_node;
    auto dirname = path->final_name;
    auto parent = path->parent_node;

    if (path->invalid_path) {
      cerr << "mkdir: error: Invalid path: " << args[i] << endl;
      return;
    } else if (node == root_dir) {
      cerr << "mkdir: error: Cannot recreate root." << endl;
      return;
    } else if (node != nullptr) {
      cerr << "mkdir: error: " << args[i] << " already exists." << endl;
      continue;
    }

    /* actually add the directory */
    parent->add_dir(dirname);
  }
}

void ToyFS::rmdir(vector<string> args) {
  ops_at_least(1);

  for (uint i = 1; i < args.size(); i++) {
    auto path = parse_path(args[i]);
    auto node = path->final_node;
    auto parent = path->parent_node;

    if (node == nullptr) {
      cerr << "rmdir: error: Invalid path: " << args[i] << endl;
    } else if (node == root_dir) {
      cerr << "rmdir: error: Cannot remove root." << endl;
    } else if (node == pwd) {
      cerr << "rmdir: error: Cannot remove working directory." << endl;
    } else if (node->contents.size() > 0) {
      cerr << "rmdir: error: Directory not empty." << endl;
    } else if (node->type != dir) {
      cerr << "rmdir: error: " << node->name << " must be directory." << endl;
    } else {
      parent->contents.remove(node);
    }
  }
}

void ToyFS::printwd(vector<string> args) {
  ops_exactly(0);

  if (pwd == root_dir) {
      cout << "/" << endl;
      return;
  }

  auto wd = pwd;
  deque<string> plist;
  while (wd != root_dir) {
    plist.push_front(wd->name);
    wd = wd->parent.lock();
  }

  for (auto dirname : plist) {
      cout << "/" << dirname;
  }
  cout << endl;
}

void ToyFS::cd(vector<string> args) {
  ops_exactly(1);

  auto path = parse_path(args[1]);
  auto node = path->final_node;

  if (node == nullptr) {
    cerr << "cd: error: Invalid path: " << args[1] << endl;
  } else if (node->type != dir) {
    cerr << "cd: error: " << args[1] << " must be a directory." << endl;
  } else {
    pwd = node;
  }
}

void ToyFS::link(vector<string> args) {
  ops_exactly(2);

  auto src_path = parse_path(args[1]);
  auto src = src_path->final_node;
  auto src_parent = src_path->parent_node;
  auto dest_path = parse_path(args[2]);
  auto dest = dest_path->final_node;
  auto dest_parent = dest_path->parent_node;
  auto dest_name = dest_path->final_name;

  if (src == nullptr) {
    cerr << "link: error: Cannot find " << args[1] << endl;
  } else if (dest != nullptr) {
    cerr << "link: error: " << args[2] << " already exists." << endl;
  } else if (src->type != file) {
    cerr << "link: error: " << args[1] << " must be a file." << endl;
  } else if (src_parent == dest_parent) {
    cerr << "link: error: src and dest must be in different directories." << endl;
  } else {
    auto new_file = DirEntry::make_de_file(dest_name, dest_parent, src->inode);
    dest_parent->contents.push_back(new_file);
  }
}

void ToyFS::unlink(vector<string> args) {
  ops_exactly(1);

  auto path = parse_path(args[1]);
  auto node = path->final_node;
  auto parent = path->parent_node;

 if (node == nullptr) {
    cerr << "unlink: error: File not found." << endl;
 } else if (node->type != file) {
    cerr << "unlink: error: " << args[1] << " must be a file." << endl;
  } else {
    parent->contents.remove(node);
  }
}

void ToyFS::stat(vector<string> args) {
  ops_at_least(1);

  for (uint i = 1; i < args.size(); i++) {
    auto path = parse_path(args[i]);
    auto node = path->final_node;

    if (node == nullptr) {
      cerr << "stat: error: " << args[i] << " not found." << endl;
    } else {
      cout << "  File: " << node->name << endl;
      if (node->type == file) {
        cout << "  Type: file" << endl;
        cout << " Inode: " << node->inode << endl;
        cout << " Links: " << node->inode.use_count() << endl;
        cout << "  Size: " << node->inode->size << endl;
        cout << "Blocks: " << node->inode->blocks_used << endl;
      } else if(node->type == dir) {
        cout << "  Type: directory" << endl;
      }
    }
  }
}

void ToyFS::ls(vector<string> args) {
  ops_exactly(0);
  for (auto dir : pwd->contents) {
    cout << dir->name << endl;
  }
}

void ToyFS::cat(vector<string> args) {
  ops_at_least(1);
}

void ToyFS::cp(vector<string> args) {
  ops_exactly(2);
}

void tree_helper(shared_ptr<DirEntry> directory, string indent) {
  auto cont = directory->contents;
  cout << directory->name << endl;
  if (cont.size() == 0) return;

  if (cont.size() >= 2) {
    auto last = *(cont.rbegin());
    for (auto entry = cont.begin(); *entry != last; entry++) {
      cout << indent << "├───";
      tree_helper(*entry, indent + "│   ");
    }
  }

  cout << indent << "└───";
  tree_helper(*(cont.rbegin()), indent + "    ");
}

void ToyFS::tree(vector<string> args) {
  ops_exactly(0);

  tree_helper(pwd, "");
}

void ToyFS::import(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::FS_export(vector<string> args) {
  ops_exactly(2);
}
