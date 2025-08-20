#!/bin/bash
#
# tmpltr Test Harness
# Comprehensive testing for all tmpltr functionality
#

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counters
TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0

# Path to tmpltr binary
TMPLTR="../src/tmpltr"

# Temporary directory for test outputs
TEST_OUTPUT_DIR="test_output_$$"
mkdir -p "$TEST_OUTPUT_DIR"

# Cleanup function
cleanup() {
    rm -rf "$TEST_OUTPUT_DIR"
}
trap cleanup EXIT

# Test execution function
run_test() {
    local test_name="$1"
    local test_cmd="$2"
    local expected_file="$3"
    local output_file="$TEST_OUTPUT_DIR/${test_name}.out"
    
    TESTS_RUN=$((TESTS_RUN + 1))
    
    echo -n "Running test: $test_name ... "
    
    # Execute the test command
    eval "$test_cmd" > "$output_file" 2>&1
    local exit_code=$?
    
    # Check if we have an expected output file
    if [ -f "$expected_file" ]; then
        if diff -q "$expected_file" "$output_file" > /dev/null 2>&1; then
            echo -e "${GREEN}PASSED${NC}"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            echo -e "${RED}FAILED${NC} (output mismatch)"
            echo "  Expected output in: $expected_file"
            echo "  Actual output in: $output_file"
            echo "  Diff:"
            diff "$expected_file" "$output_file" | head -20
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        # Just check exit code
        if [ $exit_code -eq 0 ]; then
            echo -e "${GREEN}PASSED${NC}"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            echo -e "${RED}FAILED${NC} (exit code: $exit_code)"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    fi
}

# Performance test function
run_perf_test() {
    local test_name="$1"
    local test_cmd="$2"
    local min_lines_per_sec="$3"
    
    TESTS_RUN=$((TESTS_RUN + 1))
    
    echo -n "Running performance test: $test_name ... "
    
    # Run the command and measure time
    local start_time=$(date +%s.%N)
    eval "$test_cmd" > /dev/null 2>&1
    local end_time=$(date +%s.%N)
    
    # Calculate lines per second (check if bc is available)
    if command -v bc > /dev/null 2>&1; then
        local duration=$(echo "$end_time - $start_time" | bc)
        local lines=$(wc -l < "$4")
        local lines_per_sec=$(echo "scale=0; $lines / $duration" | bc)
        
        if [ $(echo "$lines_per_sec >= $min_lines_per_sec" | bc) -eq 1 ]; then
            echo -e "${GREEN}PASSED${NC} (${lines_per_sec} lines/sec)"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            echo -e "${RED}FAILED${NC} (${lines_per_sec} lines/sec, expected >= ${min_lines_per_sec})"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        # Fallback: use awk if bc is not available
        local duration=$(awk "BEGIN {print $end_time - $start_time}")
        local lines=$(wc -l < "$4")
        local lines_per_sec=$(awk "BEGIN {printf \"%.0f\", $lines / $duration}")
        
        if [ "$lines_per_sec" -ge "$min_lines_per_sec" ]; then
            echo -e "${GREEN}PASSED${NC} (${lines_per_sec} lines/sec)"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            echo -e "${RED}FAILED${NC} (${lines_per_sec} lines/sec, expected >= ${min_lines_per_sec})"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    fi
}

echo "================================="
echo "tmpltr Test Harness"
echo "================================="
echo

# Check if tmpltr binary exists
if [ ! -x "$TMPLTR" ]; then
    echo -e "${RED}ERROR: tmpltr binary not found or not executable at $TMPLTR${NC}"
    echo "Please run 'make' first to build tmpltr"
    exit 1
fi

# =============================================================================
# BASIC FUNCTIONALITY TESTS
# =============================================================================
echo ">>> Basic Functionality Tests"
echo

# Test 1: Basic template generation
run_test "basic_template" \
    "$TMPLTR data/basic.log" \
    "expected/basic_template.out"

# Test 2: Clustering with default depth
run_test "cluster_default" \
    "$TMPLTR -c data/basic.log" \
    "expected/cluster_default.out"

# Test 3: Clustering with custom depth
run_test "cluster_depth_5" \
    "$TMPLTR -c -n 5 data/basic.log" \
    "expected/cluster_depth_5.out"

# Test 4: Template matching
run_test "template_match" \
    "$TMPLTR -m '%s %d %s' data/basic.log" \
    "expected/template_match.out"

# Test 5: Ignore quotes
run_test "ignore_quotes" \
    "$TMPLTR -g data/quoted.log" \
    "expected/ignore_quotes.out"

# =============================================================================
# FIELD TYPE DETECTION TESTS
# =============================================================================
echo
echo ">>> Field Type Detection Tests"
echo

# Test 6: Integer detection
run_test "int_detection" \
    "$TMPLTR data/integers.log" \
    "expected/int_detection.out"

# Test 7: Float detection
run_test "float_detection" \
    "$TMPLTR data/floats.log" \
    "expected/float_detection.out"

# Test 8: Hex detection
run_test "hex_detection" \
    "$TMPLTR data/hex_values.log" \
    "expected/hex_detection.out"

# Test 9: MAC address detection
run_test "mac_detection" \
    "$TMPLTR data/mac_addresses.log" \
    "expected/mac_detection.out"

# Test 10: IP address detection
run_test "ip_detection" \
    "$TMPLTR data/ip_addresses.log" \
    "expected/ip_detection.out"

# Test 11: BASE64 detection
run_test "base64_detection" \
    "$TMPLTR data/base64.log" \
    "expected/base64_detection.out"

# Test 12: Syslog date detection
run_test "syslog_date_detection" \
    "$TMPLTR data/syslog_dates.log" \
    "expected/syslog_date_detection.out"

# Test 13: ISO timestamp detection
run_test "iso_timestamp_detection" \
    "$TMPLTR data/iso_timestamps.log" \
    "expected/iso_timestamp_detection.out"

# =============================================================================
# REGRESSION TESTS FOR RECENT FIXES
# =============================================================================
echo
echo ">>> Regression Tests"
echo

# Test 14: BASE64 with padding
run_test "base64_padding" \
    "echo 'SGVsbG8gV29ybGQ=' | $TMPLTR -" \
    "expected/base64_padding.out"

# Test 15: BASE64 with special chars (+/)
run_test "base64_special_chars" \
    "echo 'dGVzdCt0ZXN0L3Rlc3Q=' | $TMPLTR -" \
    "expected/base64_special.out"

# Test 16: Asterisk handling
run_test "asterisk_handling" \
    "echo '*A****' | $TMPLTR -" \
    "expected/asterisk.out"

# Test 17: HEX case consistency
run_test "hex_case_consistency" \
    "$TMPLTR data/hex_mixed_case.log" \
    "expected/hex_case.out"

# Test 18: Syslog at line start only
run_test "syslog_line_start" \
    "$TMPLTR data/syslog_position.log" \
    "expected/syslog_position.out"

# =============================================================================
# EDGE CASES AND ERROR HANDLING
# =============================================================================
echo
echo ">>> Edge Cases and Error Handling"
echo

# Test 19: Empty file
run_test "empty_file" \
    "$TMPLTR data/empty.log" \
    "expected/empty.out"

# Test 20: Single line
run_test "single_line" \
    "echo 'test line' | $TMPLTR -" \
    "expected/single_line.out"

# Test 21: Very long lines
run_test "long_lines" \
    "$TMPLTR data/long_lines.log" \
    "expected/long_lines.out"

# Test 22: Binary data handling
run_test "binary_data" \
    "$TMPLTR data/binary_mixed.log" \
    "expected/binary_mixed.out"

# =============================================================================
# TEMPLATE FILE OPERATIONS
# =============================================================================
echo
echo ">>> Template File Operations"
echo

# Test 23: Save templates
run_test "save_templates" \
    "$TMPLTR -w $TEST_OUTPUT_DIR/templates.txt data/basic.log > /dev/null && wc -l < $TEST_OUTPUT_DIR/templates.txt" \
    "expected/template_count.out"

# Test 24: Load and ignore templates
run_test "load_templates" \
    "$TMPLTR -t data/ignore_templates.txt data/basic.log" \
    "expected/filtered.out"

# =============================================================================
# PERFORMANCE TESTS
# =============================================================================
echo
echo ">>> Performance Tests"
echo

# Generate test data if needed
if [ ! -f "data/perf_1m.log" ]; then
    echo "Generating performance test data..."
    scripts/generate_test_data.sh
fi

# Test 25: Non-clustering performance (expect >500K lines/min)
run_perf_test "perf_no_cluster" \
    "$TMPLTR data/perf_100k.log" \
    "8333" \
    "data/perf_100k.log"

# Test 26: Clustering performance (expect >100K lines/min) 
run_perf_test "perf_cluster" \
    "$TMPLTR -c data/perf_100k.log" \
    "1666" \
    "data/perf_100k.log"

# =============================================================================
# MEMORY TESTS
# =============================================================================
echo
echo ">>> Memory Tests"
echo

# Test 27: Check for memory leaks with valgrind (if available)
if command -v valgrind > /dev/null 2>&1; then
    echo -n "Running test: memory_leak_check ... "
    
    # Test if valgrind works first
    valgrind --version > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo -e "${YELLOW}SKIPPED${NC} (valgrind setup issue)"
        TESTS_RUN=$((TESTS_RUN + 1))
    else
        # Run the actual memory check
        valgrind_output=$(valgrind --leak-check=full --error-exitcode=1 --track-origins=yes $TMPLTR data/basic.log 2>&1 > /dev/null)
        valgrind_exit=$?
        
        # Check for valgrind startup errors
        if echo "$valgrind_output" | grep -q "Fatal error at startup"; then
            echo -e "${YELLOW}SKIPPED${NC} (missing glibc debug symbols)"
            echo "  Hint: Install libc6-dbg (Debian/Ubuntu) or glibc-debuginfo (RHEL/Fedora)"
        elif [ $valgrind_exit -eq 0 ]; then
            echo -e "${GREEN}PASSED${NC} (no memory leaks detected)"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            echo -e "${RED}FAILED${NC} (memory issues detected)"
            echo "  Valgrind output:"
            echo "$valgrind_output" | head -10
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
        TESTS_RUN=$((TESTS_RUN + 1))
    fi
else
    echo "Skipping memory tests (valgrind not installed)"
fi

# =============================================================================
# TEST SUMMARY
# =============================================================================
echo
echo "================================="
echo "Test Summary"
echo "================================="
echo "Tests run:    $TESTS_RUN"
echo -e "Tests passed: ${GREEN}$TESTS_PASSED${NC}"
echo -e "Tests failed: ${RED}$TESTS_FAILED${NC}"

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed!${NC}"
    exit 1
fi