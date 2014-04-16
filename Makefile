# A makefile
CXX = clang++
CFLAGS = --std=c++11 -Wall -Wextra -g

default: main

main: main.cpp toyfs.o
	$(CXX) $(CFLAGS) -o main main.cpp toyfs.o

toyfs.o: toyfs.cpp toyfs.hpp
	$(CXX) $(CFLAGS) -c toyfs.cpp

clean:
	@rm -rf main *.o
