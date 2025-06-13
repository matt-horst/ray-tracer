SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
CC=g++
CFLAGS=-Wall -Wpedantic -Werror -std=c++23 -g

HEADERS=$(shell find $(SRC_DIR) -name '*.hpp')
SRCS=$(filter-out test_,$(shell find $(SRC_DIR) -name '*.cpp'))
TEST_SRCS=$(shell find $(SRC_DIR) -name 'test_*.cpp')
TEST_BINS=$(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%,$(TEST_SRCS))

all: $(BIN_DIR) $(OBJ_DIR) $(BIN_DIR)/main

$(BIN_DIR)/main: $(SRC_DIR)/render.cpp $(SRC_DIR)/main.cpp $(SRC_DIR)/interval.cpp $(HEADERS)
	$(CC) $(CFLAGS) $(SRC_DIR)/render.cpp $(SRC_DIR)/main.cpp $(SRC_DIR)/interval.cpp  -o $@ 

$(OBJ_DIR)/test_%.o: $(SRC_DIR)/test_%.cpp $(SRC_DIR)/%.hpp
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/test_%:  $(OBJ_DIR)/test_%.o
	$(CC) -o $@ $<

tests: $(TEST_BINS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(OBJ_DIR)/* $(BIN_DIR)/*
