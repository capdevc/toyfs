#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "toyfs.hpp"

#define PRMPT "sh> "
#define DISKSIZE 102400
#define BLOCKSIZE 1024

int test_fs(const std::string filename) {
    ToyFS(filename, DISKSIZE, BLOCKSIZE);
    return 0;
}

void repl(const std::string filename) {
  using ::std::string;
  using ::std::vector;
  using ::std::cout;
  using ::std::cin;
  using ::std::endl;
  using ::std::cerr;
  using ::std::istringstream;
  using ::std::getline;

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
            std::cout << "unknown command: " << args[0] << std::endl;
        }
        cout << PRMPT;
    }

    delete(fs);
    return;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

#ifdef DEBUG
    test_fs(std::string(argv[1]));
#else
    repl(std::string(argv[1]));
#endif
    return 0;
}

