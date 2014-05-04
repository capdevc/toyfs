# A makefile
CXX = clang++
CFLAGS = --std=c++11 -Wall -Wextra -g -DDEBUG

default: main

main: main.cpp toyfs.o direntry.o inode.o
	$(CXX) $(CFLAGS) -o main main.cpp direntry.o toyfs.o inode.o

toyfs.o: toyfs.cpp toyfs.hpp
	$(CXX) $(CFLAGS) -c toyfs.cpp

direntry.o: direntry.cpp direntry.hpp
	$(CXX) $(CFLAGS) -c direntry.cpp

inode.o: inode.cpp inode.hpp
	$(CXX) $(CFLAGS) -c inode.cpp

clean:
	@rm -rf main *.o
