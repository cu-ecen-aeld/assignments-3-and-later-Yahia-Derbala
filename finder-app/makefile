# Makefile for writer application

# Compiler
CC = gcc

# Cross-compiler prefix (default to empty, meaning native compilation)
CROSS_COMPILE ?=

# Compiler flags
CFLAGS = -Wall -Wextra

# Source files
SRC = writer.c

# Output binary
OUTPUT = writer

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC)

clean:
	rm -f $(OUTPUT)

.PHONY: all clean

