#  _____  _____  ___  ______ _____
# /  ___||  _  |/ _ \ | ___ \  ___|
# \ `--. | | | / /_\ \| |_/ / |__
#  `--. \| | | |  _  ||    /|  __|
# /\__/ /\ \_/ / | | || |\ \| |___
# \____/  \___/\_| |_/\_| \_\____/
# Makefile

# COMPILER
CC = gcc
AR = ar

# EXECUTABLE NAME
LIBRARY = libsoare1.a
EXE = soare

# DIRECTORIES
SOURCE_DIR = src
BINARY_DIR = bin
LIBRARY_DIR = lib
INCLUDE_DIR = include
APPLICATION_SOURCE_DIR = app

# FLAGS
CFLAGS = -Wall -Wextra -Waddress
MAIN = $(APPLICATION_SOURCE_DIR)/*.c

SRC_FILES := $(wildcard $(SOURCE_DIR)/*.c)
OBJ_FILES := $(patsubst $(SOURCE_DIR)/%.c,$(BINARY_DIR)/%.o,$(SRC_FILES))

.PHONY: all

all: makedir $(EXE) clean

makedir:
	mkdir -p $(LIBRARY_DIR)
	mkdir -p $(BINARY_DIR)

$(BINARY_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) -c $< -o $@ -I $(INCLUDE_DIR) $(CFLAGS)

$(LIBRARY): $(OBJ_FILES)
	$(AR) -rc $(LIBRARY_DIR)/$(LIBRARY) $(OBJ_FILES)

$(EXE): $(LIBRARY)
	$(CC) $(MAIN) -o $(BINARY_DIR)/$(EXE) -I $(INCLUDE_DIR) -L $(LIBRARY_DIR) -lsoare1

clean:
	rm $(OBJ_FILES)
