CC = g++
CFLAGS = -Wall -Wextra -Iinclude -Iexternal/xxhash -g
LDFLAGS = -Lexternal/xxhash -lxxhash -lm #lm per math.h

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
EXECUTABLE = $(BIN_DIR)/sraster

SRC_DIR = src
MAIN_FILE = $(SRC_DIR)/main.cpp
SRC_FILES = $(filter-out $(MAIN_FILE), $(wildcard $(SRC_DIR)/*.cpp))
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

TEST_DIR = test
TEST_SRC_FILES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_EXECUTABLES = $(patsubst $(TEST_DIR)/%.cpp,$(TEST_DIR)/bin/%,$(TEST_SRC_FILES))

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES) $(OBJ_DIR)/main.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/main.o: $(MAIN_FILE) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TEST_DIR)/bin/:
	mkdir -p $(TEST_DIR)/bin/

$(TEST_DIR)/bin/%: $(TEST_DIR)/%.cpp
	$(CC) $(CFLAGS) $< $(OBJ_FILES) -o $@ $(LDFLAGS) -lcriterion

clean:
	rm -rf $(BUILD_DIR)

# test: $(TEST_DIR)/bin/ $(TEST_EXECUTABLES)
# 	for test in $(TEST_EXECUTABLES) ; do ./$$test ; done
