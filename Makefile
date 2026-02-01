
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
CORE = core
MODULES = modules
INCLUDE = include


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


CORE_OBJS := $(patsubst $(CORE)/%.c, $(BIN)/%.o, $(wildcard $(CORE)/*.c))
MODULES_OBJS := $(patsubst $(MODULES)/%.c, $(BIN)/%.o, $(wildcard $(MODULES)/*.c))


.PHONY: all
all: $(BIN)/$(BUILD)


$(BIN):

	@echo - Make bin directory...
	mkdir $(BIN)


$(RES):

	@echo - Build Windows ressources...
	$(WINDRES) $(RC) -O coff -o $(RES)


$(BIN)/%.o: $(CORE)/%.c

	$(CC) -c $< -o $@ -I $(INCLUDE) $(CFLAGS) $(SOARE_FLAGS)


$(BIN)/%.o: $(MODULES)/%.c

	$(CC) -c $< -o $@ -I $(INCLUDE) $(CFLAGS) $(MODULE_FLAGS)


$(BIN)/$(BUILD): $(BIN) $(RES) $(CORE_OBJS) $(MODULES_OBJS)

ifeq ($(OS), Windows_NT)
	@echo - Build SOARE DLLs Windows...
	$(CC) -shared $(CORE_OBJS) -o $(BIN)/soare$(VERSION_MAJOR).dll -I $(INCLUDE) $(CFLAGS)
else
	@echo - Build SOARE static libraries Unix/macOS...
	$(AR) rcs $(BIN)/libsoare$(VERSION_MAJOR).a $(CORE_OBJS)
endif

	@echo - Build SOARE interpreter...
	$(CC) $(RES) $(APP)/*.c $(MODULES)/*.c -o $(BIN)/$(BUILD) -I $(INCLUDE) -L$(BIN) -lsoare$(VERSION_MAJOR) $(CFLAGS) $(INTERPRETER_FLAGS)

	@echo - Remove useless compiled files...
	rm $(BIN)/*.o


.PHONY: run
run: $(BIN)/$(BUILD)

	@echo - Run SOARE...
	$(BIN)/$(BUILD)


.PHONY: clean
clean: $(BIN)

	@echo - Remove compiled files...
	rm -rf $(BIN)


.PHONY: help
help:

	@echo
	@echo Available targets :
	@echo ---
	@echo - make all : Build SOARE 
	@echo - make run : Build and run SOARE
	@echo - make help : Show this help message
	@echo - make clean : Remove compiled files
	@echo

