#  _____  _____  ___  ______ _____
# /  ___||  _  |/ _ \ | ___ \  ___|
# \ `--. | | | / /_\ \| |_/ / |__
#  `--. \| | | |  _  ||    /|  __|
# /\__/ /\ \_/ / | | || |\ \| |___
# \____/  \___/\_| |_/\_| \_\____/
# Antoine LANDRIEUX
# GNU General Public License v3.0
# https://www.gnu.org/licenses/
# SOARE Makefile

# COMPILER
CC = gcc
AR = ar

# OUTPUT
LIBRARY = libsoare1.a

# DIRECTORIES
SOURCE_DIR = src
BINARY_DIR = bin
LIBRARY_DIR = lib
INCLUDE_DIR = include

# FLAGS
CFLAGS = -Wall -Wextra -Waddress

SRC_FILES := $(wildcard $(SOURCE_DIR)/*.c)
OBJ_FILES := $(patsubst $(SOURCE_DIR)/%.c,$(BINARY_DIR)/%.o,$(SRC_FILES))

.PHONY: all

all: makedir $(LIBRARY) clean

makedir:
	mkdir -p $(LIBRARY_DIR)
	mkdir -p $(BINARY_DIR)

$(BINARY_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) -c $< -o $@ -I $(INCLUDE_DIR) $(CFLAGS)

$(LIBRARY): $(OBJ_FILES)
	$(AR) -rc $(LIBRARY_DIR)/$(LIBRARY) $(OBJ_FILES)

clean:
	rm -r $(BINARY_DIR)
