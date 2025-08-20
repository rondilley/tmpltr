# Standalone Makefile for tmpltr tests
# Can be used independently of the main build system

TMPLTR = ../src/tmpltr
SHELL = /bin/bash

.PHONY: all test clean generate verbose perf regression

# Default target runs all tests
all: test

# Run all tests
test:
	@chmod +x run_tests.sh
	@./run_tests.sh

# Generate test data and expected outputs
generate:
	@chmod +x create_expected_outputs.sh scripts/generate_test_data.sh
	@./create_expected_outputs.sh

# Run tests with verbose output
verbose:
	@chmod +x run_tests.sh
	@bash -x ./run_tests.sh

# Run only performance tests
perf:
	@chmod +x run_tests.sh
	@./run_tests.sh | grep -A100 "Performance Tests"

# Run only regression tests
regression:
	@chmod +x run_tests.sh
	@./run_tests.sh | grep -A100 "Regression Tests"

# Clean up test artifacts
clean:
	@rm -rf test_output_* data/perf_*.log
	@echo "Test artifacts cleaned"

# Help target
help:
	@echo "tmpltr Test Suite"
	@echo "================="
	@echo "Available targets:"
	@echo "  make test       - Run all tests"
	@echo "  make generate   - Generate test data and expected outputs"
	@echo "  make verbose    - Run tests with verbose output"
	@echo "  make perf       - Run only performance tests"
	@echo "  make regression - Run only regression tests"
	@echo "  make clean      - Remove test artifacts"
	@echo "  make help       - Show this help message"