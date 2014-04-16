#include <string>
#include "toyfs.hpp"

int test_fs(const std::string filename) {
  ToyFS(filename, 100000, 1024);
  return 0;
}

int main(int argc, char **argv) {
  test_fs(std::string(argv[1]));
}
