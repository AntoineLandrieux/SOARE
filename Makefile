
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
VERSION_MAJOR = 1

AR = ar
CC = gcc

BIN = bin
LIB = lib
SRC = src
CORE = core
INCLUDE = include

CFLAGS := -Wall
CFLAGS += -Wextra
CFLAGS += -Wno-unused-result
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-implicit-fallthrough
CFLAGS += -O2

ifeq ($(OS), Windows_NT)

WINDRES = windres
RC = windows/resources/app.rc
RES = $(BIN)/resources.res

else

WINDRES = echo

endif

SOARE_FLAGS :=
SOARE_FLAGS += -D __SOARE_COLORED_OUTPUT
# SOARE_FLAGS += -D __SOARE_DEBUG

default: $(BIN)/$(APP)

$(LIB)/libsoare$(VERSION_MAJOR).a: $(CORE_OBJS)

CORE_OBJS := $(patsubst $(CORE)/%.c, $(LIB)/%.o, $(wildcard $(CORE)/*.c))

$(BIN):
	mkdir $(BIN)

$(LIB):
	mkdir $(LIB)

$(RES):
	$(WINDRES) $(RC) -O coff -o $(RES)

$(LIB)/%.o: $(CORE)/%.c
	$(CC) -c $< -o $@ -I $(INCLUDE) $(CFLAGS) $(SOARE_FLAGS)

$(BIN)/$(APP): $(BIN) $(LIB) $(RES) $(CORE_OBJS)
	$(AR) rcs $(LIB)/libsoare$(VERSION_MAJOR).a $(CORE_OBJS)
	$(CC) $(RES) $(SRC)/*.c -o $(BIN)/$(APP) -I $(INCLUDE) -L$(LIB) -lsoare$(VERSION_MAJOR) $(CFLAGS) $(SOARE_FLAGS)
	rm $(CORE_OBJS)

run:
	$(BIN)/$(APP)

clean:
	rm -rf $(BIN) $(LIB)
