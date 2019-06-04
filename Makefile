# Project name
TARGET = libndef

# Directories
BIN_DIR = bin
INC_DIR = include
OBJ_DIR = obj
SRC_DIR = src
TST_DIR = test
DBG_DIR = dbg
DOC_DIR = doc

# Compiler flags
# Enable all warning checks and treat warnings as errors
CXXFLAGS += -Wall -Wextra -Werror
# Project is compiled with C++11 support
CXXFLAGS += -std=c++11
# Include project header files
CXXFLAGS += -I$(INC_DIR)/

# Max optimization
RELEASE_FLAGS += -O3
# Enable AddressSanitize support
RELEASE_FLAGS += -fsanitize=address
# Enable Link Time Optimization
RELEASE_FLAGS += -flto
# Disable all assertions
RELEASE_FLAGS += -DNDEBUG

# Linker flags - all below are the same as above
LDFLAGS  	  	+= -Wall -Wextra -Werror
RELEASE_LDFLAGS += -fsanitize=address -flto

# Libraries
LIB  :=

# Check for clang support and use it if available
ifneq (, $(shell which clang++))
	CXX = clang++
endif

# Source and header files
SRC  	 = $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRC = $(wildcard $(TST_DIR)/*.cpp)
OBJ  	 = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DBG_OBJ  = $(SRC:$(SRC_DIR)/%.cpp=$(DBG_DIR)/%.o)
TEST_OBJ = $(TEST_SRC:$(TST_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Executabls
EXEC = $(BIN_DIR)/$(TARGET)
DBG  = $(BIN_DIR)/$(TARGET)_dbg
TEST = $(BIN_DIR)/$(TARGET)_test

.PHONY: all clean debug test doc

all : $(EXEC)
debug : $(DBG)
test : $(TEST)
clean :
	-$(RM) -r $(OBJ) $(EXEC) $(DBG_DIR) $(BIN_DIR) $(OBJ_DIR)
	@echo "Cleaned all binaries and object files"

# --- Documentation ---

DOXYGEN_VERSION := $(shell doxygen --version 2> /dev/null) 

doc : $(SRC) Doxyfile
ifdef DOXYGEN_VERSION
	mkfile_path=$(abspath $(lastword $(MAKEFILE_LIST)))
	current_dir=$(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
	@echo "Found Doxygen version $(DOXYGEN_VERSION)"
	@echo "Generating documentation via Doxyfile $(current_dir)/Doxyfile"
	doxygen
	@xdg-open doc/html/index.html
else
	$(error "No doxygen in PATH, unable to generate documentation!")
endif

# --- Release ---

$(OBJ) : $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir $(OBJ_DIR) ||:
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -c $< -o $@ $(LIB)
	@echo "Compiled "$<" successfully!"

$(EXEC) : $(OBJ)
	@mkdir $(BIN_DIR) ||:
	$(CXX) $(LDFLAGS) $(RELEASE_LDFLAGS) -o $@ $(OBJ) $(LIB)
	@echo "Linking complete!"
	@echo "Binary available at" $(EXEC)

# --- Debug ---

$(DBG_OBJ) : $(DBG_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir $(DBG_DIR) ||:
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LIB)
	@echo "[DEBUG] Compiled "$<" successfully!"

$(DBG) : $(DBG_OBJ)
	@mkdir $(BIN_DIR) ||:
	$(CXX) $(LDFLAGS) -o $@ $(DBG_OBJ) $(LIB)
	@echo "[DEBUG] Linking complete!"
	@echo "[DEBUG] Binary available at" $(DBG)

# --- Test ---

$(TEST_OBJ) : $(OBJ_DIR)/%.o : $(TST_DIR)/%.cpp
	@mkdir $(OBJ_DIR) ||:
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LIB)
	@echo "[TEST] Compiled "$<" successfully!"

$(TEST) : $(TEST_OBJ) $(DBG_OBJ)
	@mkdir $(BIN_DIR) ||:
	$(CXX) $(LDFLAGS) -o $@ $(TEST_OBJ) $(DBG_OBJ) $(LIB)
	@echo "[TEST] Linking complete!"
	@echo "[TEST] Binary available at" $(TEST)
	@echo "[TEST] Executing test" $(TEST)
	@$(TEST)