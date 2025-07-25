# Top-level Makefile for building with CMake
# Usage:
#   make         - builds in build/ directory
#   make clean   - removes build directory
#   make ctest   - run tests via ctest (old way most likely incomplete)
#   make doctest - run doctests (new way complete, test every function)
#   make all     - does build, doctest, and clean
#   make rebuild - does clean, then build
#   make remake  - does exactly rebuild

# Build directory
BUILD_DIR := build

# Binary directory
BIN_DIR := bin

# Default target
build:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@cd $(BUILD_DIR) && cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON && make -j6

# Clean build
clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@rm -rf __pycache__ *.egg-info
	@find . -name "*.pyc" -delete
	@find . -type d -name "__pycache__" -exec rm -r {} +

# Run Tests via ctest
ctest:
	@make build
	@cd $(BUILD_DIR) && ctest --output-on-failure

# Run doctest
doctest: 
	@if [ ! -d $(BUILD_DIR) ]; then \
		echo "Build directory not found. Running make rebuild..."; \
		$(MAKE) rebuild; \
	else \
		echo "Build directory found. Skipping rebuild."; \
	fi
	
	@cd $(BIN_DIR) && ./doctest --no-breaks --success --no-line-numbers --no-path-in-filenames

# make all
all: build doctest clean install

# rebuild
rebuild: clean build
remake: rebuild

# set python interpreter
PYTHON ?= python3

# Install Python module (editable mode recommended during dev)
installe:
	cd $(CURDIR) && $(PYTHON) -m pip install -e .

install:
	cd $(CURDIR) && $(PYTHON) -m pip install -r requirements.txt

testpy:
	pytest --rich python_package/test/ -v -s

# help and usage
help:
	@echo "Usage:"
	@echo "  make [target]"
	@echo
	@echo "Available targets:"
	@echo "  build       - Configure and build project in ./build/"
	@echo "  clean       - Remove build/ and bin/ directories"
	@echo "  ctest       - Run CTest-based tests (older method, may be incomplete)"
	@echo "  doctest     - Run doctests (recommended, tests all functions)"
	@echo "  all         - Run build, doctest, and then clean"
	@echo "  rebuild     - Clean then build"
	@echo "  remake      - Alias for rebuild"
	@echo "  install     - Install Python Module"
	@echo "  installe    - Install Python Module (dev mode)"
	@echo "  help        - Show this help message"


.PHONY: installe install
