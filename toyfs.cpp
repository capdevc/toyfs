#include "toyfs.hpp"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <assert.h>

using std::cerr;
using std::cout;
using std::endl;
using std::istringstream;
using std::fstream;
using std::make_pair;
using std::make_shared;
using std::max;
using std::min;
using std::pair;
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
             const uint block_size,
             const uint direct_blocks)
    : filename(filename),
      fs_size(fs_size * block_size),
      block_size(block_size),
      direct_blocks(direct_blocks),
      num_blocks(ceil(static_cast<double>(fs_size) / block_size)) {

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

bool ToyFS::getMode(Mode *mode, string mode_s) {
  if (mode_s == "w") {
    *mode = W;
  } else if(mode_s == "r") {
    *mode = R;
  } else if (mode_s == "rw") {
    *mode = RW;
  } else {
    return false;
  }
  return true;
}

bool ToyFS::basic_open(Descriptor *d, vector <string> args) {
  assert(args.size() == 3);

  Mode mode;
  auto path = parse_path(args[1]);
  auto node = path->final_node;
  auto parent = path->parent_node;
  bool known_mode = getMode(&mode, args[2]);

  if (path->invalid_path == true) {
    cerr << args[0] << ": error: Invalid path: " << args[1] << endl;
  } else if(!known_mode) {
    cerr << args[0] << ": error: Unknown mode: " << args[2] << endl;
  } else if (node == nullptr && (mode == R || mode == RW)) {
    cerr << args[0] << ": error: " << args[1] << " does not exist." << endl;
  } else if (node != nullptr && node->type == dir) {
    cerr << args[0] << ": error: Cannot open a directory." << endl;
  } else {
    //create the file if necessary
    if(node == nullptr) {
      node = parent->add_file(path->final_name);
    }

    // get a descriptor
    uint fd = next_descriptor++;
    *d = Descriptor{mode, 0, node->inode, fd};
    open_files[fd] = *d;
    return true;
  }
  return false;
}

void ToyFS::open(vector<string> args) {
  ops_exactly(2);
  Descriptor desc;
  if (basic_open(&desc, args)) {
    cout << desc.fd << endl;
  }
}

void ToyFS::read(vector<string> args) {
  ops_exactly(2);

  uint fd;
  if ( !(istringstream(args[1]) >> fd)) {
    cerr << "read: error: Unknown descriptor." << endl;
  } else {
    auto desc = open_files.find(fd);
    if (desc == open_files.end()) {
      cerr << "read: error: File descriptor not open." << endl;
    } else if(desc->second.mode != R && desc->second.mode != RW) {
      cerr << "read: error: " << args[1] << " not open for read." << endl;
    } else {
      //TODO: read from file
    }
  }
}

void ToyFS::write(vector<string> args) {
  ops_exactly(2);

  uint fd;
  if ( !(istringstream(args[1]) >> fd)) {
    cerr << "write: error: Unknown descriptor." << endl;
  } else {
    auto desc = open_files.find(fd);
    if (desc == open_files.end()) {
      cerr << "write: error: File descriptor not open." << endl;
    } else if(desc->second.mode != W && desc->second.mode != RW) {
      cerr << "write: error: " << args[1] << " not open for write." << endl;
    } else if (!basic_write(desc->second, args[2])) {
      cerr << "write: error: Insufficient disk space or file too large." << endl;
    }
  }
}

uint ToyFS::basic_write(Descriptor &desc, string data) {
  const char *bytes = data.c_str();
  uint &pos = desc.byte_pos;
  uint bytes_to_write = data.size();
  uint bytes_written = 0;
  auto inode = desc.inode.lock();
  uint &file_size = inode->size;
  uint &file_blocks_used = inode->blocks_used;
  uint new_size = max(file_size, pos + bytes_to_write);

  // find space
  vector<pair<uint, uint>> free_chunks;
  uint blocks_needed = ceil(static_cast<double>(new_size)/block_size) - file_blocks_used;
  auto fl_it = begin(free_list);
  while (blocks_needed > 0) {
    if (fl_it == end(free_list)) {
      // 0 return because we ran out of free space
      return 0;}
    if (fl_it->num_blocks > blocks_needed) {
      // we found a chunk big enough to hold the rest of our write
      free_chunks.push_back(make_pair(fl_it->pos, blocks_needed));
      fl_it->pos += blocks_needed;
      fl_it->num_blocks -= blocks_needed;
      break;
    }
    // we have a chunk, but will fill it and need more
    free_chunks.push_back((make_pair(fl_it->pos, fl_it->num_blocks)));
    auto used_entry = fl_it++;
    free_list.erase(used_entry);
  }

  // allocate our blocks
  for (auto &fc_it : free_chunks) {
    uint block_pos = fc_it.first;
    uint num_blocks = fc_it.second;
    for (uint i = 0; i < num_blocks; ++i, block_pos += block_size) {
      if (file_blocks_used++ < direct_blocks) {
        inode->d_blocks.push_back(block_pos);
      } else {
        uint index = (file_blocks_used - direct_blocks) / direct_blocks;
        inode->i_blocks->at(index).push_back(block_pos);
      }
    }
  }

  // actually write our blocks
  uint dbytes = direct_blocks * block_size - 1;
  while (bytes_to_write > 0) {
    uint write_size = min({block_size, block_size - pos % block_size, bytes_to_write});
    uint write_dest = 0;
    if (pos < dbytes) {
      write_dest = inode->d_blocks[pos / block_size] + pos % block_size;
    } else {
      uint i = (pos - dbytes) / (block_size * block_size);
      uint j = pos / block_size;
      write_dest = inode->i_blocks->at(i)[j] + pos % block_size;
    }
    disk_file.seekp(write_dest);
    disk_file.write(bytes + bytes_written, write_size);
    bytes_written += write_size;
    bytes_to_write -= write_size;
    pos += write_size;
  }

  file_size = new_size;
  return bytes_written;
}

void ToyFS::seek(vector<string> args) {
  ops_exactly(2);
  uint fd;
  if ( !(istringstream(args[1]) >> fd)) {
    cerr << "seek: error: Unknown descriptor." << endl;
  } else {
    auto desc = open_files.find(fd);
    if (desc == open_files.end()) {
      cerr << "seek: error: File descriptor not open." << endl;
    } else {
      //TODO: seek file
    }
  }
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
        cout << " Inode: " << node->inode.get() << endl;
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

  for(uint i = 1; i < args.size(); i++) {
    Descriptor desc;
    if(!basic_open(&desc, vector<string> {args[0], args[i], "r"})) {
      /* failed to open */
      continue;
    }
    
    auto size = desc.inode.lock()->size;
    read(vector<string>
            {args[0], std::to_string(desc.fd), std::to_string(size)});
    open_files.erase(desc.fd);
  }
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
  
  Descriptor desc;
  std::ifstream in(args[1]);
  if(!in.is_open()) {
    cerr << args[0] << ": error: Unable to open " << args[1] << endl;
  } else if (basic_open(&desc, vector<string>{args[0], args[2], "w"})) {
    string line;
    while (getline(in, line)) {
      desc.inode.lock()->write(line);
    }
    open_files.erase(desc.fd);
  }
}

void ToyFS::FS_export(vector<string> args) {
  ops_exactly(2);

  std::ofstream out(args[2]);
  if (!out.is_open()) {
    cerr << args[0] << ": error: Unable to open " << args[2] << endl;
  } else {
    std::streambuf *coutbuf = cout.rdbuf();
    cat(vector<string> {args[0], args[1]});
    cout.rdbuf(coutbuf);
  }
}
