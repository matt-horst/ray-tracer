SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
CC=g++
CFLAGS=-Wall -Wpedantic -Werror -std=c++23

HEADERS=$(shell find $(SRC_DIR) -name '*.hpp')
TEST_SRCS=$(shell find $(SRC_DIR) -name 'test_*.cpp')
TEST_BINS=$(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%,$(TEST_SRCS))

all: $(BIN_DIR) $(OBJ_DIR) $(BIN_DIR)/main

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@ 

$(BIN_DIR)/main: $(OBJ_DIR)/main.o
	$(CC) $< -o $@

$(OBJ_DIR)/test_vec3.o: $(SRC_DIR)/test_vec3.cpp $(SRC_DIR)/vec3.hpp
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/test_vec3:  $(OBJ_DIR)/test_vec3.o
	$(CC) -o $@ $<

tests: $(TEST_BINS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(OBJ_DIR)/* $(BIN_DIR)/*
