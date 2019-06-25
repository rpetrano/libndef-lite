# Project name
TARGET = libndef

# Directories
BIN_DIR = bin
INC_DIR = include
OBJ_DIR = obj
SRC_DIR = src
TST_DIR = test
DOC_DIR = doc
COV_DIR = coverage_report

# Compiler flags
# Enable all warning checks
CXXFLAGS += -Wall -Wextra
# Project is compiled with C++14 support
CXXFLAGS += -std=c++14
# Include project header files
CXXFLAGS += -I$(INC_DIR)/

# Max optimization
RELEASE_FLAGS += -O3
# Enable AddressSanitize support
RELEASE_FLAGS += -fsanitize=address
# Enable Link Time Optimization
RELEASE_FLAGS += -flto
# Treat warnings as errors for release builds
RELEASE_FLAGS += -Werror
# Code is memory location independent
RELEASE_FLAGS += -fPIC

# Debug flags
DEBUG_FLAGS    = -o0 -ggdb3
COVERAGE_FLAGS := $(DEBUG_FLAGS) -fno-inline -fno-inline-small-functions -fno-default-inline --coverage

# Linker flags - all below are the same as above
LDFLAGS  	  	:= -Wall -Wextra
RELEASE_LDFLAGS := $(LDFLAGS) -fsanitize=address -flto -fPIC

# Libraries
LIB  :=

# Source and header files
SRC  	 = $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRC = $(wildcard $(TST_DIR)/*.cpp)
OBJ  	 = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DBG_OBJ  = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.dbg.o)
TEST_OBJ = $(TEST_SRC:$(TST_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Executables 
EXEC = $(BIN_DIR)/$(TARGET)
DBG  = $(BIN_DIR)/$(TARGET)_dbg
TEST = $(BIN_DIR)/$(TARGET)_test

.PHONY: all clean debug test doc runtests coverage partial-clean

all : $(EXEC)
debug : $(DBG)
test : CXXFLAGS += -Itest/
test : $(TEST)
partial-clean:
	-$(shell find obj ! -name 'test-main.*' -type f -exec rm -f {} +)
	-$(RM) -r $(EXEC) $(DBG) $(TEST) $(COV_DIR)
	@echo "Cleaned all binaries and object files (ignored test-main)"
clean:
	-$(RM) -r $(EXEC) $(DBG) $(TEST) $(COV_DIR) $(DBG_OBJ) $(OBJ) $(TEST_OBJ)
	@echo "Cleaned all binaries and object files"

# --- Coverage ---

coverage : LDFLAGS += -lgcov --coverage
coverage : DEBUG_FLAGS = $(COVERAGE_FLAGS)
coverage : $(TEST) runtests
	@mkdir $(COV_DIR) ||:
	lcov --directory obj/ --capture --output-file /tmp/libndef-lite_coverage.lcov -b .
	lcov --remove /tmp/libndef-lite_coverage.lcov -o $(COV_DIR)/coverage.lcov '/usr/include/*' \
		'/usr/lib/*' \
		'$(PWD)/include/doctest.hpp'
	genhtml -o $(COV_DIR)/ $(COV_DIR)/coverage.lcov

# --- Documentation ---

DOXYGEN_VERSION := $(shell doxygen --version 2> /dev/null) 

doc : $(SRC) Doxyfile
ifdef DOXYGEN_VERSION
	@echo "Found Doxygen version $(DOXYGEN_VERSION)"
	@echo "Generating documentation via Doxyfile $(PWD)/Doxyfile"
	doxygen
	@xdg-open doc/html/index.html
else
	$(error "No doxygen in PATH, unable to generate documentation!")
endif

# --- Release ---

$(OBJ) : $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir $(OBJ_DIR) ||:
	$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(EXEC) : $(OBJ)
	@mkdir $(BIN_DIR) ||:
	$(CXX) $(LDFLAGS) $(RELEASE_LDFLAGS) -o $@ $(OBJ) $(LIB)
	@echo "Linking complete!"
	@echo "Binary available at" $(EXEC)

# --- Debug ---

$(DBG_OBJ) : $(OBJ_DIR)/%.dbg.o : $(SRC_DIR)/%.cpp
	@mkdir $(OBJ_DIR) ||:
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -c $< -o $@
	@echo "[DEBUG] Compiled "$<" successfully!"

$(DBG) : $(DBG_OBJ)
	@mkdir $(BIN_DIR) ||:
	$(CXX) $(LDFLAGS) $(LDFLAGS) -o $@ $(DBG_OBJ) $(LIB)
	@echo "[DEBUG] Linking complete!"
	@echo "[DEBUG] Binary available at" $(DBG)

# --- Test ---

$(TEST_OBJ) : $(OBJ_DIR)/%.o : $(TST_DIR)/%.cpp
	@mkdir $(OBJ_DIR) ||:
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -c $< -o $@
	@echo "[TEST] Compiled "$<" successfully!"

$(TEST) : $(TEST_OBJ) $(DBG_OBJ)
	@mkdir $(BIN_DIR) ||:
	$(CXX) $(LDFLAGS) $(LDFLAGS) -o $@ $(TEST_OBJ) $(DBG_OBJ) $(LIB)
	@echo "[TEST] Linking complete!"
	@echo "[TEST] Binary available at" $(TEST)

runtests : $(TEST)
	@echo "[TEST] Executing test" $(TEST)
	@$(TEST)
