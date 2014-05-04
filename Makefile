# A makefile
CXX = clang++
CFLAGS = --std=c++11 -Wall -Wextra -g -DDEBUG

default: main

main: main.cpp toyfs.o direntry.o
	$(CXX) $(CFLAGS) -o main main.cpp toyfs.o direntry.o

toyfs.o: toyfs.cpp toyfs.hpp
	$(CXX) $(CFLAGS) -c toyfs.cpp

direntry.o: direntry.cpp direntry.hpp
	$(CXX) $(CFLAGS) -c direntry.cpp

clean:
	@rm -rf main *.o
