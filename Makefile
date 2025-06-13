SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
CC=g++
CFLAGS=-Wall -Wpedantic -Werror -std=c++23 -g

HEADERS=$(shell find $(SRC_DIR) -name '*.hpp')
ALL_SRCS=$(shell find $(SRC_DIR) -name '*.cpp')
SRCS=$(filter-out $(SRC_DIR)/test_%,$(ALL_SRCS))
OBJS=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TEST_SRCS=$(filter $(SRC_DIR)/test_%,$(ALL_SRCS))
TEST_BINS=$(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%,$(TEST_SRCS))

all: $(BIN_DIR) $(OBJ_DIR) $(BIN_DIR)/main

$(BIN_DIR)/main: $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/test_%:  $(OBJ_DIR)/test_%.o $(OBJ_DIR)/%.o
	$(CC) -o $@ $^

$(BIN_DIR)/test_%:  $(OBJ_DIR)/test_%.o
	$(CC) -o $@ $<

tests: $(TEST_BINS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(OBJ_DIR)/* $(BIN_DIR)/*
