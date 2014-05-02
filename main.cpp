#include <string>
#include <iostream>
#include <sstream>
#include "toyfs.hpp"

#define PRMPT "sh> "
#define DISKSIZE 102400
#define BLOCKSIZE 1024

int test_fs(const std::string filename) {
    ToyFS(filename, DISKSIZE, BLOCKSIZE);
    return 0;
}

void repl(const std::string filename) {
    using namespace std;
    
    ToyFS *fs = new ToyFS(filename, DISKSIZE, BLOCKSIZE);

    string cmd;
    vector<string> args;
    string token;
    
    cout << PRMPT;
    while(getline(cin, cmd)) {
        args.clear();
        istringstream iss(cmd);
        while (iss >> token) { args.push_back(token); }
        if(args.size() == 0) {
            cout << PRMPT;
            continue;
        }

        if(args[0] == "mkfs") {
            delete(fs);
            fs = new ToyFS(filename, DISKSIZE, BLOCKSIZE);
        } else if(args[0] == "open") {
        } else if(args[0] == "read") {
        } else if(args[0] == "write") {
        } else if(args[0] == "seek") {
        } else if(args[0] == "close") {
        } else if(args[0] == "mkdir") {
        } else if(args[0] == "rmdir") {
        } else if(args[0] == "cd") {
        } else if(args[0] == "link") {
        } else if(args[0] == "unlink") {
        } else if(args[0] == "stat") {
        } else if(args[0] == "ls") {
        } else if(args[0] == "cat") {
        } else if(args[0] == "cp") {
        } else if(args[0] == "tree") {
        } else if(args[0] == "import") {
        } else if(args[0] == "export") {
        } else if(args[0] == "exit") {
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
    if(argc != 2) {
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

