#
# makefile
# Declan Hopkins
# 10/19/2015
#

CC = g++
SRC_PATH = src
BIN_PATH = bin
FRAMEWORK_PATH = /Library/Frameworks
FRAMEWORKS = -framework SDL2 -framework SDL2_mixer
BIN_NAME = gb-emu
C_FLAGS = -Wall -std=c++14
SRC_FILES := $(wildcard $(SRC_PATH)/*.cpp)
OBJ_FILES := $(SRC_FILES:$(SRC_PATH)%.cpp=$(BIN_PATH)%.o)

# Build the project in either debug or release
all: debug

# Build the project and package it into an app bundle
release: clean build
	# Does nothing currently
	@echo "*** Release build complete ***"

# Build the project and run it through the terminal
debug: clean build_header build
	@echo "*** Debug build complete ***"
	@./$(BIN_PATH)/$(BIN_NAME)

# Precompile header
build_header:
	@$(CC) -w -c src/PCH.hpp -o src/PCH.hpp.gch -F $(FRAMEWORK_PATH) $(FRAMEWORKS)

# Link all the .o files in the bin/ directory to create the executable
build: $(OBJ_FILES)
	@echo "*** Linking ***"
	@$(CC) $(OBJ_FILES) -o $(BIN_PATH)/$(BIN_NAME) -F $(FRAMEWORK_PATH) $(FRAMEWORKS)

# Compile all the .cpp files in the src/ directory to the bin/ directory
$(BIN_PATH)/%.o: $(SRC_PATH)/%.cpp
	@echo "*** Compiling" $< "***"
	@$(CC) -F $(FRAMEWORK_PATH) -c $< -o $@ $(C_FLAGS)

# Clean up all the raw binaries
clean:
	@echo "*** Cleaning Binaries ***"
	@rm -f $(BIN_PATH)/$(BIN_NAME) $(BIN_PATH)/*.o
