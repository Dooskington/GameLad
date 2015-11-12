#
# makefile
# Declan Hopkins
# 10/19/2015
#

CC = g++
FRAMEWORK_PATH = /Library/Frameworks
FRAMEWORKS = -framework SDL2
BIN_NAME = gb-emu
C_FLAGS = -Wall -std=c++14

SRC_PATH = gb-emu
BIN_PATH = gb-emu_bin
SRC_FILES := $(wildcard $(SRC_PATH)/*.cpp)
OBJ_FILES := $(SRC_FILES:$(SRC_PATH)%.cpp=$(BIN_PATH)%.o)

LIB_SRC_PATH = gb-emu-lib
LIB_BIN_PATH = gb-emu-lib_bin
LIB_SRC_FILES := $(wildcard $(LIB_SRC_PATH)/*.cpp)
LIB_OBJ_FILES := $(LIB_SRC_FILES:$(LIB_SRC_PATH)%.cpp=$(LIB_BIN_PATH)%.o)

# Build the emulator and run it
all: lib emu
	@echo "*** Build complete ***"

# Build the emulator library. This is required for the base emulator.
lib: build_lib
	@echo "*** gb-emu-lib Built ***"

build_lib: $(LIB_OBJ_FILES)
	@echo "*** Building gb-emu-lib ***"
	@ar -r $(LIB_BIN_PATH)/libgb-emu.a $(LIB_OBJ_FILES)

$(LIB_BIN_PATH)/%.o: $(LIB_SRC_PATH)/%.cpp
	@echo "*** Compiling" $< "***"
	@$(CC) -F $(FRAMEWORK_PATH) -c $< -o $@ $(C_FLAGS)

# Build the emulator, using gb-emu-lib
emu: build_emu
	@echo "*** gb-emu built ***"
	@./$(BIN_PATH)/$(BIN_NAME)

build_emu: $(OBJ_FILES)
	@echo "*** Building gb-emu ***"
	@$(CC) $(OBJ_FILES) -o $(BIN_PATH)/$(BIN_NAME) -F $(FRAMEWORK_PATH) $(FRAMEWORKS) -L$(LIB_BIN_PATH) -I$(LIB_BIN_PATH) -lgb-emu

$(BIN_PATH)/%.o: $(SRC_PATH)/%.cpp
	@echo "*** Compiling" $< "***"
	@$(CC) -I$(LIB_SRC_PATH) -F $(FRAMEWORK_PATH) -c $< -o $@ $(C_FLAGS)

# Clean up all the raw binaries
clean:
	@echo "*** Cleaning Binaries ***"
	@rm -f $(LIB_BIN_PATH)/*.o
	@rm -f $(BIN_PATH)/$(BIN_NAME) $(BIN_PATH)/*.o
