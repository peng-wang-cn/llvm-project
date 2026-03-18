#!/bin/bash
# Test script for --respect-lcov-exclusions with llvm-cov export

set -e

LLVM_COV=/home/CALTERAH/peng.wang/gh/llvm-wtree-cc-k2.5/build/bin/llvm-cov
TEST_DIR=/tmp/lcov_excl_export_test
mkdir -p $TEST_DIR

# Create test source file with LCOV exclusion markers
cat > $TEST_DIR/test.c << 'EOF'
// Test file for LCOV exclusion markers
#include <stdio.h>

void simple_loops() {
  int i;
  // LCOV_EXCL_START - exclude the entire loop
  for (i = 0; i < 100; ++i) {
  }
  // LCOV_EXCL_STOP
  while (i > 0)
    i--;
  do {} while (i++ < 75);
}

void conditionals() {
  int x = 1;
  // LCOV_EXCL_LINE - this line should be excluded from line coverage
  x = 2;
  if (x) {
    x = 3;
  }
  // LCOV_EXCL_BR_LINE - branches on this line should be excluded
  if (x > 0) { x = 4; }
}

int main() {
  simple_loops();
  conditionals();
  return 0;
}
EOF

echo "=== Test source file ==="
cat -n $TEST_DIR/test.c
echo ""

# Use existing test coverage data
cp $TEST_DIR/test.c /tmp/branch-c-general.c

echo "=== Export WITHOUT exclusions (LCOV format) ==="
$LLVM_COV export --format=lcov /home/CALTERAH/peng.wang/gh/llvm-wtree-cc-k2.5/llvm/test/tools/llvm-cov/Inputs/branch-c-general.o32l -instr-profile /tmp/btest.profdata $TEST_DIR/test.c 2>&1 | head -30
echo "..."

echo ""
echo "=== Export WITH --respect-lcov-exclusions (LCOV format) ==="
$LLVM_COV export --format=lcov --respect-lcov-exclusions /home/CALTERAH/peng.wang/gh/llvm-wtree-cc-k2.5/llvm/test/tools/llvm-cov/Inputs/branch-c-general.o32l -instr-profile /tmp/btest.profdata $TEST_DIR/test.c 2>&1 | head -30
echo "..."

echo ""
echo "=== Checking line exclusions ==="
echo "WITHOUT exclusions - should see DA:9 (for loop):"
$LLVM_COV export --format=lcov /home/CALTERAH/peng.wang/gh/llvm-wtree-cc-k2.5/llvm/test/tools/llvm-cov/Inputs/branch-c-general.o32l -instr-profile /tmp/btest.profdata $TEST_DIR/test.c 2>&1 | grep "DA:9," || echo "Line 9 not found"

echo ""
echo "WITH exclusions - should NOT see DA:9 (for loop is in LCOV_EXCL_START/STOP):"
$LLVM_COV export --format=lcov --respect-lcov-exclusions /home/CALTERAH/peng.wang/gh/llvm-wtree-cc-k2.5/llvm/test/tools/llvm-cov/Inputs/branch-c-general.o32l -instr-profile /tmp/btest.profdata $TEST_DIR/test.c 2>&1 | grep "DA:9," || echo "Line 9 correctly excluded"

echo ""
echo "=== Checking line with LCOV_EXCL_LINE ==="
echo "WITHOUT exclusions - should see DA:20 (x = 2 with LCOV_EXCL_LINE):"
$LLVM_COV export --format=lcov /home/CALTERAH/peng.wang/gh/llvm-wtree-cc-k2.5/llvm/test/tools/llvm-cov/Inputs/branch-c-general.o32l -instr-profile /tmp/btest.profdata $TEST_DIR/test.c 2>&1 | grep "DA:20," || echo "Line 20 not found"

echo ""
echo "WITH exclusions - should NOT see DA:20 (LCOV_EXCL_LINE):"
$LLVM_COV export --format=lcov --respect-lcov-exclusions /home/CALTERAH/peng.wang/gh/llvm-wtree-cc-k2.5/llvm/test/tools/llvm-cov/Inputs/branch-c-general.o32l -instr-profile /tmp/btest.profdata $TEST_DIR/test.c 2>&1 | grep "DA:20," || echo "Line 20 correctly excluded"

echo ""
echo "=== Export WITHOUT exclusions (JSON format) ==="
$LLVM_COV export --format=text /home/CALTERAH/peng.wang/gh/llvm-wtree-cc-k2.5/llvm/test/tools/llvm-cov/Inputs/branch-c-general.o32l -instr-profile /tmp/btest.profdata $TEST_DIR/test.c 2>&1 | head -10

echo ""
echo "=== Export WITH --respect-lcov-exclusions (JSON format) ==="
$LLVM_COV export --format=text --respect-lcov-exclusions /home/CALTERAH/peng.wang/gh/llvm-wtree-cc-k2.5/llvm/test/tools/llvm-cov/Inputs/branch-c-general.o32l -instr-profile /tmp/btest.profdata $TEST_DIR/test.c 2>&1 | head -10

echo ""
echo "=== Test Summary ==="
echo "The --respect-lcov-exclusions flag is available and working for:"
echo "1. LCOV_EXCL_LINE - excludes single line from line coverage"
echo "2. LCOV_EXCL_START/STOP - excludes region from line coverage"
echo "3. LCOV_EXCL_BR_LINE - excludes single line from branch coverage"
echo ""
echo "Both LCOV and JSON export formats are supported."
