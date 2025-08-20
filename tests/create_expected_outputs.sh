#!/bin/bash
#
# Create expected output files for tmpltr tests
# Run this after confirming tmpltr is working correctly
#

TMPLTR="../src/tmpltr"

# Check if tmpltr exists
if [ ! -x "$TMPLTR" ]; then
    echo "Error: tmpltr binary not found at $TMPLTR"
    exit 1
fi

# Generate test data first
echo "Generating test data..."
chmod +x scripts/generate_test_data.sh
scripts/generate_test_data.sh

# Create expected directory
mkdir -p expected

echo "Creating expected output files..."

# Basic functionality
$TMPLTR data/basic.log > expected/basic_template.out
$TMPLTR -c data/basic.log > expected/cluster_default.out
$TMPLTR -c -n 5 data/basic.log > expected/cluster_depth_5.out
$TMPLTR -m '%t INFO User %s logged in from %i' data/basic.log > expected/template_match.out
$TMPLTR -g data/quoted.log > expected/ignore_quotes.out

# Field type detection
$TMPLTR data/integers.log > expected/int_detection.out
$TMPLTR data/floats.log > expected/float_detection.out
$TMPLTR data/hex_values.log > expected/hex_detection.out
$TMPLTR data/mac_addresses.log > expected/mac_detection.out
$TMPLTR data/ip_addresses.log > expected/ip_detection.out
$TMPLTR data/base64.log > expected/base64_detection.out
$TMPLTR data/syslog_dates.log > expected/syslog_date_detection.out
$TMPLTR data/iso_timestamps.log > expected/iso_timestamp_detection.out

# Regression tests
echo 'SGVsbG8gV29ybGQ=' | $TMPLTR - > expected/base64_padding.out
echo 'dGVzdCt0ZXN0L3Rlc3Q=' | $TMPLTR - > expected/base64_special.out
echo '*A****' | $TMPLTR - > expected/asterisk.out
$TMPLTR data/hex_mixed_case.log > expected/hex_case.out
$TMPLTR data/syslog_position.log > expected/syslog_position.out

# Edge cases
$TMPLTR data/empty.log > expected/empty.out
echo 'test line' | $TMPLTR - > expected/single_line.out
$TMPLTR data/long_lines.log > expected/long_lines.out
$TMPLTR data/binary_mixed.log > expected/binary_mixed.out

# Template operations
$TMPLTR -w /tmp/test_templates.txt data/basic.log > /dev/null
wc -l < /tmp/test_templates.txt > expected/template_count.out
rm -f /tmp/test_templates.txt

$TMPLTR -t data/ignore_templates.txt data/basic.log > expected/filtered.out

# Memory test expected
echo "No leaks" > expected/no_leaks.out

echo "Expected output files created!"
echo
echo "Please review the expected outputs to ensure they are correct."
echo "If any outputs look wrong, fix tmpltr and regenerate."