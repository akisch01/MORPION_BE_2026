# Makefile for MORPION_BE_2026

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Isrc
LDFLAGS =

# Directories
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Files
TARGET = $(BUILD_DIR)/morpion
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Default target
all: $(TARGET)

# Linking the executable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compiling source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to run the executable
run: all
	./$(TARGET)

# Clean rule
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all run clean
