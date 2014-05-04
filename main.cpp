#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "toyfs.hpp"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::istringstream;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

const string PRMPT = "sh> ";
const uint DISKSIZE = 102400;
const uint BLOCKSIZE = 1024;

int test_fs(const string filename) {
  ToyFS myfs(filename, DISKSIZE, BLOCKSIZE);

  vector<string> args1 = {"mkdir /dir-1"};
  vector<string> args2 = {"mkdir /dir-1/dir-b"};
  vector<string> args3 = {"mkdir dir-2"};
  vector<string> args4 = {"mkdir dir-2/dir-b"};
  vector<string> args5 = {"mkdir dir-2/dir-b/dir-deep"};
  myfs.mkdir(args1);
  myfs.mkdir(args2);
  myfs.mkdir(args3);
  myfs.mkdir(args4);
  myfs.mkdir(args5);

  return 0;
}

void repl(const string filename) {

    ToyFS *fs = new ToyFS(filename, DISKSIZE, BLOCKSIZE);

    string cmd;
    vector<string> args;
    string token;

    cout << PRMPT;
    while (getline(cin, cmd)) {
        args.clear();
        istringstream iss(cmd);
        while (iss >> token) { args.push_back(token); }
        if (args.size() == 0) {
            cout << PRMPT;
            continue;
        }

        if (args[0] == "mkfs") {
            if (args.size() == 1) {
                delete(fs);
                fs = new ToyFS(filename, DISKSIZE, BLOCKSIZE);
            } else {
                cerr << "mkfs: too many operands" << endl;
            }
        } else if (args[0] == "open") {
            fs->open(args);
        } else if (args[0] == "read") {
            fs->close(args);
        } else if (args[0] == "write") {
            fs->write(args);
        } else if (args[0] == "seek") {
            fs->seek(args);
        } else if (args[0] == "close") {
            fs->close(args);
        } else if (args[0] == "mkdir") {
            fs->mkdir(args);
        } else if (args[0] == "rmdir") {
            fs->rmdir(args);
        } else if (args[0] == "cd") {
            fs->cd(args);
        } else if (args[0] == "link") {
            fs->link(args);
        } else if (args[0] == "unlink") {
            fs->unlink(args);
        } else if (args[0] == "stat") {
            fs->stat(args);
        } else if (args[0] == "ls") {
            fs->ls(args);
        } else if (args[0] == "cat") {
            fs->cat(args);
        } else if (args[0] == "cp") {
            fs->cp(args);
        } else if (args[0] == "tree") {
            fs->tree(args);
        } else if (args[0] == "import") {
            fs->import(args);
        } else if (args[0] == "export") {
            fs->FS_export(args);
        } else if (args[0] == "exit") {
            break;
        } else {
            cout << "unknown command: " << args[0] << endl;
        }
        cout << PRMPT;
    }

    delete(fs);
    return;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "usage: " << argv[0] << " filename" << endl;
        return 1;
    }

#ifdef DEBUG
    test_fs(string(argv[1]));
#else
    repl(string(argv[1]));
#endif
    return 0;
}

