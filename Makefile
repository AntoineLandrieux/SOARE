
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


BUILD = soare
VERSION_MAJOR = 1


AR = ar
CC = gcc


APP = app
BIN = bin
LIB = lib
CORE = core
MODULES = modules
INCLUDE = include


TEST = test
EXAMPLES = examples


CFLAGS := -Wall
CFLAGS += -Wextra
CFLAGS += -Wno-unused-result
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


MODULE_FLAGS :=


INTERPRETER_FLAGS :=
# INTERPRETER_FLAGS += -D __KILL_MESSAGE=\"" - Bye\""
# INTERPRETER_FLAGS += -D __SOARE_DEBUG


CORE_OBJS := $(patsubst $(CORE)/%.c, $(LIB)/%.o, $(wildcard $(CORE)/*.c))


TEST_OBJS := $(wildcard $(TEST)/*.soare)
# TEST_OBJS += $(wildcard $(EXAMPLES)/*.soare)


.PHONY: all
all: $(BIN)/$(BUILD)


$(BIN):

	@echo - Make bin directory...
	mkdir $(BIN)


$(LIB):

	@echo - Make lib directory...
	mkdir $(LIB)


$(RES):

	@echo - Build Windows resources...
	$(WINDRES) $(RC) -O coff -o $(RES)


$(LIB)/%.o: $(CORE)/%.c

	$(CC) -c $< -o $@ -I $(INCLUDE) $(CFLAGS) $(SOARE_FLAGS)


$(BIN)/$(BUILD): $(BIN) $(LIB) $(RES) $(CORE_OBJS)

	@echo - Build SOARE static libraries...
	$(AR) rcs $(LIB)/libsoare$(VERSION_MAJOR).a $(CORE_OBJS)

	@echo - Build SOARE interpreter...
	$(CC) $(RES) $(APP)/*.c $(MODULES)/*.c -o $(BIN)/$(BUILD) -I $(INCLUDE) -L$(LIB) -lsoare$(VERSION_MAJOR) $(CFLAGS) $(INTERPRETER_FLAGS)

	@echo - Remove useless compiled files...
	rm $(LIB)/*.o


.PHONY: run
run:

	@echo - Run SOARE...
	$(BIN)/$(BUILD)


.PHONY: test
test:

	@echo - Run SOARE tests...
	$(BIN)/$(BUILD) $(TEST_OBJS)


.PHONY: clean
clean: $(BIN) $(LIB)

	@echo - Remove compiled files...
	rm -rf $(BIN) $(LIB)


.PHONY: help
help:

	@echo
	@echo Available targets :
	@echo ---
	@echo - make all : Build SOARE 
	@echo - make run : Run SOARE
	@echo - make help : Show this help message
	@echo - make test : Test SOARE with test files
	@echo - make clean : Remove compiled files
	@echo

