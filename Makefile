
#
#  _____  _____  ___  ______ _____
# /  ___||  _  |/ _ \ | ___ \  ___|
# \ `--. | | | / /_\ \| |_/ / |__
#  `--. \| | | |  _  ||    /|  __|
# /\__/ /\ \_/ / | | || |\ \| |___
# \____/  \___/\_| |_/\_| \_\____/
#
# Antoine LANDRIEUX (MIT License) <Makefile>
# <https://github.com/AntoineLandrieux/SOARE/>
#

APP = soare
VERSION_MAJ = 1

AR = ar
CC = gcc
CPP = g++

BIN = bin
LIB = lib
SRC = src
CORE = core
INCLUDE = include

CFLAGS = -Wall -Wextra -Wpedantic -Werror

ifeq ($(OS), Windows_NT)
WINDRES = windres
RES = $(BIN)/res.o
else
WINDRES = echo
endif

# DEBUG = -D __SOARE_DEBUG
# NO_COLORED_OUTPUT = -D __SOARE_NO_COLORED_OUTPUT

default: $(BIN)/$(APP)

$(LIB)/libsoare.a: $(CORE_OBJS)

CORE_OBJS := $(patsubst $(CORE)/%.c, $(LIB)/%.o, $(wildcard $(CORE)/*.c))

$(LIB):
	mkdir -p $(LIB)

$(LIB)/%.o: $(CORE)/%.c
	$(CC) -c $< -o $@ -I $(INCLUDE) $(CFLAGS) $(DEBUG) $(NO_COLORED_OUTPUT)

$(BIN)/$(APP): $(LIB) $(CORE_OBJS) $(SRC)/Main.cpp
	mkdir -p $(BIN)
	$(WINDRES) windows/resources/app.rc -coff $(RES)
	$(AR) rcs $(LIB)/libsoare$(VERSION_MAJ).a $(CORE_OBJS)
	$(CPP) $(RES) $(SRC)/Main.cpp -o $(BIN)/$(APP) -I $(INCLUDE) -L$(LIB) -lsoare$(VERSION_MAJ) $(CFLAGS) $(DEBUG) $(NO_COLORED_OUTPUT)
	rm $(CORE_OBJS) $(RES)

run:
	$(BIN)/$(APP)

clean:
	rm -rf $(BIN) $(LIB)
