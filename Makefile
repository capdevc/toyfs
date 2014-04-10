# A makefile
CXX = clang++
SRC_PATH = src
BIN_PATH = bin
INC_PATH = include
CFLAGS = --std=c++11 -Wall -Wextra -g
INCLUDE = -I $(SRC_PATH) -I $(INC_PATH)

default: $(BIN_PATH)/main

bin/main: $(SRC_PATH)/main.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -o $(BIN_PATH)/main $(SRC_PATH)/main.cpp

clean:
	@rm -rf $(BIN_PATH)/* $(SRC_PATH)/*.o
