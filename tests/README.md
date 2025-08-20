# tmpltr Test Suite

Comprehensive test harness for validating all tmpltr functionality.

## Quick Start

From the main tmpltr directory after building:
```bash
make test           # Run all tests
make test-verbose   # Run with verbose output
make test-generate  # Generate expected outputs
```

From the tests directory:
```bash
make test       # Run all tests
make generate   # Generate test data and expected outputs
make verbose    # Run with verbose output
```

## Test Categories

### 1. Basic Functionality Tests
- Template generation
- Clustering with various depths
- Template matching
- Quote handling

### 2. Field Type Detection Tests
- Integer detection (%d)
- Float detection (%f)
- Hexadecimal detection (%x)
- MAC address detection (%m)
- IP address detection (%i, %I)
- BASE64 detection (%b)
- Syslog date detection (%D)
- ISO timestamp detection (%t)

### 3. Regression Tests
Tests for recently fixed issues:
- BASE64 padding handling
- BASE64 special characters (+, /)
- Asterisk (*) character handling
- HEX case consistency
- Syslog date position requirements

### 4. Edge Cases
- Empty files
- Single line inputs
- Very long lines (4KB+)
- Binary data mixed with text

### 5. Template File Operations
- Saving templates to file (-w)
- Loading and ignoring templates (-t)
- Template filtering

### 6. Performance Tests
- Non-clustering performance (target: >10M lines/min)
- Clustering performance (target: >1M lines/min)

### 7. Memory Tests
- Memory leak detection (requires valgrind)

## Directory Structure

```
tests/
├── run_tests.sh              # Main test runner
├── create_expected_outputs.sh # Generate expected outputs
├── Makefile                  # Standalone test Makefile
├── README.md                 # This file
├── data/                     # Test input data
│   ├── basic.log
│   ├── base64.log
│   ├── hex_values.log
│   └── ...
├── expected/                 # Expected output files
│   ├── basic_template.out
│   ├── cluster_default.out
│   └── ...
└── scripts/                  # Test helper scripts
    └── generate_test_data.sh

```

## Adding New Tests

1. Add test data to `data/` directory
2. Add test case to `run_tests.sh`:
   ```bash
   run_test "test_name" \
       "$TMPLTR options data/input.log" \
       "expected/output.out"
   ```
3. Generate expected output:
   ```bash
   ../src/tmpltr options data/input.log > expected/output.out
   ```
4. Re-run tests to verify

## Test Output

Tests produce colored output:
- 🟢 **PASSED** - Test succeeded
- 🔴 **FAILED** - Test failed (shows diff)
- 🟡 **SKIPPED** - Test skipped (missing dependencies)

## Continuous Integration

The test suite can be integrated with CI systems:
```bash
make test || exit 1  # Exits with error code on failure
```

## Troubleshooting

### Test Failures
1. Check the diff output to see what changed
2. Verify if the change is intentional
3. If intentional, regenerate expected outputs:
   ```bash
   make test-generate
   ```

### Performance Test Failures
- Performance tests may fail on slow systems
- Adjust thresholds in `run_tests.sh` if needed
- Use `make test-perf` to run only performance tests

### Memory Test Failures
- Requires valgrind to be installed
- May report false positives with some libraries
- Use suppression files if needed

## Dependencies

- bash 4.0+
- bc (for performance calculations)
- diff (for output comparison)
- valgrind (optional, for memory tests)
- perl (for test data generation)