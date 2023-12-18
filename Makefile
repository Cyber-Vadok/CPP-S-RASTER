CC = g++
CFLAGS = -Wall -Wextra -Iinclude -g
LDFLAGS = -lm

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
EXECUTABLE = $(BIN_DIR)/sraster

SRC_DIR = src
MAIN_FILE = $(SRC_DIR)/main.cpp
SRC_FILES = $(filter-out $(MAIN_FILE), $(wildcard $(SRC_DIR)/*.cpp))
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES) $(OBJ_DIR)/main.o | $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/main.o: $(MAIN_FILE) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(BUILD_DIR)
