#include "toyfs.hpp"
#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

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

#define ops_at_least(x)                                         \
  if (static_cast<int>(args.size()) < x+1) {                    \
    cerr << args[0] << ": missing operand" << endl;             \
    return;                                                     \
  }

#define ops_less_than(x)                                        \
  if (static_cast<int>(args.size()) > x+1) {                    \
    cerr << args[0] << ": too many operands" << endl;           \
    return;                                                     \
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
}

void ToyFS::mkdir(vector<string> args) {
  ops_at_least(1);
  // just to see
#ifdef DEBUG
  cout << "args: " << args[1] << endl;
#endif

  /* add each new directory one at a time */
  for(uint i = 1; i < args.size(); i++) {
      auto where = pwd;
      
      /* remove initial '/' */
      if (args[i][0] == '/') {
        args[i].erase(0,1);
        where = root_dir;
      }
      
      /* figure out new name and path */
      auto path_tokens = parse_path(args[i]);
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
      bool not_new = false;
      for(auto dir : where->contents) {
          if(dir->name == new_dir_name) {
              cerr << new_dir_name << " already exists" << endl;
              not_new = true;
              break;
          }
      }
      if(not_new) { continue; }

      /* actually add the directory */
      where->add_dir(new_dir_name);
#ifdef DEBUG
      cout << "adding " << new_dir_name << " in: " << where->name << endl;
      cout << "---" << endl;
#endif
    }
}

void ToyFS::printwd(vector<string> args) {
    ops_exactly(0);
    cout << pwd->name << endl;
}

void ToyFS::rmdir(vector<string> args) {
  ops_at_least(1);
  auto where = pwd;
}

void ToyFS::cd(vector<string> args) {
  ops_exactly(1);

  auto where = pwd;
  if(args[1][0] == '/') {
      args[1].erase(0,1);
      where = root_dir;
  }
  auto path_tokens = parse_path(args[1]);
  auto chg_dir_name = path_tokens.back();
  if(path_tokens.size() >= 2) {
      path_tokens.pop_back();
      where = find_file(where, path_tokens);
  }
  
  if(where != nullptr) {
      for(auto dir : where->contents) {
          if(dir->name == chg_dir_name) {
              pwd = dir;
              return;
          }
      }
      cerr << chg_dir_name << " not found" << endl; 
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
