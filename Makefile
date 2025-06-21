SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
INC_DIR=include
LIB_DIR=lib
CC=g++
CFLAGS=-Wall -Wpedantic -Werror -std=c++23 -g -I $(INC_DIR) -fPIC
LDFLAGS=-L lib -static -lyaml-cpp -fPIE

MAINS=$(SRC_DIR)/bouncing_spheres.cpp

HEADERS=$(shell find $(SRC_DIR) -name '*.hpp')
ALL_SRCS=$(shell find $(SRC_DIR) -name '*.cpp')
SRCS=$(filter-out $(SRC_DIR)/test_%,$(ALL_SRCS))
SRCS:=$(filter-out $(MAINS),$(SRCS))
OBJS=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TEST_SRCS=$(filter $(SRC_DIR)/test_%,$(ALL_SRCS))
TEST_BINS=$(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%,$(TEST_SRCS))

all: $(BIN_DIR) $(OBJ_DIR) $(BIN_DIR)/main

$(BIN_DIR)/main: $(OBJS) $(HEADERS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(BIN_DIR)/bouncing_spheres: $(OBJ_DIR)/bouncing_spheres.o $(OBJ_DIR)/rtw_stb_image.o
	$(CC) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp
	$(CC) -c $< $(CFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< $(CFLAGS) -o $@

$(OBJ_DIR)/test_%.o: $(SRC_DIR)/test_%.cpp $(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp
	$(CC) -c $< $(CFLAGS) -o $@

$(OBJ_DIR)/test_%.o: $(SRC_DIR)/test_%.cpp $(SRC_DIR)/%.hpp
	$(CC) -c $< $(CFLAGS) -o $@

$(BIN_DIR)/test_%:  $(OBJ_DIR)/test_%.o $(OBJ_DIR)/%.o
	$(CC) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/test_%:  $(OBJ_DIR)/test_%.o
	$(CC) $< $(LDFLAGS) -o $@

$(BIN_DIR)/test_serialization: $(OBJ_DIR)/test_serialization.o $(OBJ_DIR)/rtw_stb_image.o
	$(CC) $^ $(LDFLAGS) -o $@

tests: $(TEST_BINS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(OBJ_DIR)/* $(BIN_DIR)/*
