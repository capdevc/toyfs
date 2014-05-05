#include "toyfs.hpp"
#include <cmath>
#include <iostream>
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
using std::vector;
using std::weak_ptr;
using std::deque;

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


vector<string> parse_path(string path_str) {
  istringstream is(path_str);
  string token;
  vector<string> tokens;

  while (getline(is, token, '/')) {
    tokens.push_back(token);
  }
  return tokens;
}

ToyFS::ToyFS(const string& filename,
             const uint fs_size,
             const uint block_size)
    : filename(filename),
      fs_size(fs_size),
      block_size(block_size),
      num_blocks(ceil(fs_size / block_size)) {

  root_dir = make_shared<DirEntry>(DirEntry("root", root_dir));
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

// walk the dir tree from start, returning a pointer to the file
// or directory specified in path_str
shared_ptr<DirEntry> ToyFS::find_file(const shared_ptr<DirEntry> &start,
                                      const vector<string> &path_tokens) {
  auto entry = start;
  for (auto &tok : path_tokens) {
    entry = entry->find_child(tok);
    if (entry == nullptr) {
      return entry;
    }
  }
  return entry;
}

void ToyFS::open(vector<string> args) {
  ops_exactly(2);
  uint mode;
  istringstream(args[2]) >> mode;
  auto where = pwd;
  if (args[1][0] == '/') {
    args[1].erase(0,1);
    where = root_dir;
  }

  auto path_tokens = parse_path(args[1]);
  auto file_name = path_tokens.back();

  // walk the input until we have the right dir
  if (path_tokens.size() >= 2) {
    path_tokens.pop_back();
    where = find_file(where, path_tokens);
  }
  if (where == nullptr) {
    cerr << "Invalid path or something like that." << endl;
    return;
  }

  auto file = find_file(where, vector<string>{file_name});
  // make sure we have a file, or explain why not
  if (file == nullptr) {
    if (mode == 1) {
      cout << "File does not exist." << endl;
      return;
    } else {
      file = where->add_file(file_name);
    }
  }
  if (file->type == dir) {
    cout << "Cannot open a directory." << endl;
    return;
  }

  // get a descriptor
  uint fd = next_descriptor++;
  open_files[fd] = Descriptor{mode, 0, file->inode};
  cout << fd << endl;
  return;
}

void ToyFS::read(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::write(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::seek(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::close(vector<string> args) {
  ops_exactly(1);
  uint fd;
  istringstream(args[1]) >> fd;
  open_files.erase(fd);
}

void ToyFS::mkdir(vector<string> args) {
  ops_at_least(1);
  /* add each new directory one at a time */
  for (uint i = 1; i < args.size(); i++) {
    auto where = pwd;

    /* remove initial '/' */
    if (args[i][0] == '/') {
      args[i].erase(0,1);
      where = root_dir;
    }

    /* figure out new name and path */
    auto path_tokens = parse_path(args[i]);
    if(path_tokens.size() == 0) {
        cerr << "cannot recreate root" << endl;
        return;
    }
    auto new_dir_name = path_tokens.back();
    if (path_tokens.size() >= 2) {
      path_tokens.pop_back();
      where = find_file(where, path_tokens);
    }
    if (where == nullptr) {
      cerr << "Invalid path or something like that" << endl;
      return;
    }

    /* check that this directory doesn't exist */
    auto file = find_file(where, vector<string>{new_dir_name});
    if (file != nullptr) {
        cerr << new_dir_name << " already exists" << endl;
        continue;
    }

    /* actually add the directory */
    where->add_dir(new_dir_name);
#ifdef DEBUG
    cout << "adding " << new_dir_name << " in: " << where->name << endl;
    cout << "---" << endl;
#endif
  }
}

void ToyFS::rmdir(vector<string> args) {
  ops_at_least(1);

  auto rm_dir = pwd;
  if (args[1][0] == '/') {
    args[1].erase(0,1);
    rm_dir = root_dir;
  }
  
  auto path_tokens = parse_path(args[1]);
  rm_dir = find_file(rm_dir, path_tokens);

    if(rm_dir == nullptr) {
        cerr << "Invalid path" << endl;
    } else if(rm_dir == root_dir) {
        cerr << "rmdir: error: cannot remove root" << endl;
    } else if(rm_dir == pwd) {
        cerr << "rmdir: error: cannot remove working directory" << endl;
    } else if(rm_dir->contents.size() > 0) {
        cerr << "rmdir: error: directory not empty" << endl;
    } else if(rm_dir->type != dir) {
        cerr << "rmdir: error: " << rm_dir->name << " must be directory\n";
    } else {
        auto parent = rm_dir->parent.lock();
        parent->contents.remove(rm_dir);
    }
}

void ToyFS::printwd(vector<string> args) {
  ops_exactly(0);

  if(pwd == root_dir) {
      cout << "/" << endl;
      return;
  }
  
  auto wd = pwd;
  deque<string> plist;
  while(wd != root_dir) {
    plist.push_front(wd->name);
    wd = wd->parent.lock();
  }

  for(auto dirname : plist) {
      cout << "/" << dirname;
  }
  cout << endl;
}

void ToyFS::cd(vector<string> args) {
  ops_exactly(1);

  auto where = pwd;
  if (args[1][0] == '/') {
    args[1].erase(0,1);
    where = root_dir;
  }

  auto path_tokens = parse_path(args[1]);
  where = find_file(where, path_tokens);
  
  if(where != nullptr) {
    pwd = where;
  } else {
    cerr << "Invalid path" << endl;
  }
}


void ToyFS::link(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::unlink(vector<string> args) {
  ops_exactly(1);
}

void ToyFS::stat(vector<string> args) {
  ops_at_least(1);
}

void ToyFS::ls(vector<string> args) {
  ops_exactly(0);
  for(auto dir : pwd->contents) {
    cout << dir->name << endl;
  }
}

void ToyFS::cat(vector<string> args) {
  ops_at_least(1);
}

void ToyFS::cp(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::tree(vector<string> args) {
  ops_exactly(0);
}

void ToyFS::import(vector<string> args) {
  ops_exactly(2);
}

void ToyFS::FS_export(vector<string> args) {
  ops_exactly(2);
}
