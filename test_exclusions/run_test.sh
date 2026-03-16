#!/bin/bash
# Test script for --respect-lcov-exclusions in llvm-cov export

set -e

LLVM_COV=/home/CALTERAH/peng.wang/gh/llvm-project/build/bin/llvm-cov
LLVM_PROFDATA=/home/CALTERAH/peng.wang/gh/llvm-project/build/bin/llvm-profdata
TEST_DIR=/home/CALTERAH/peng.wang/gh/llvm-project/test_exclusions
cd $TEST_DIR

echo "=== Test: Verify --respect-lcov-exclusions option exists ==="
$LLVM_COV export --help | grep -q "respect-lcov-exclusions" && echo "PASS: Option exists" || { echo "FAIL: Option not found"; exit 1; }

echo ""
echo "=== Test: Export without exclusions (baseline) ==="
# Use existing test input: merge proftext and export
$LLVM_PROFDATA merge /home/CALTERAH/peng.wang/gh/llvm-project/llvm/test/tools/llvm-cov/Inputs/branch-showBranchPercentage.proftext -o %t.profdata
BASELINE_OUTPUT=$($LLVM_COV export --format=text /home/CALTERAH/peng.wang/gh/llvm-project/llvm/test/tools/llvm-cov/Inputs/branch-showBranchPercentage.o32l -instr-profile=%t.profdata 2>/dev/null)

# Check that we have line coverage data
echo "$BASELINE_OUTPUT" | grep -q '"lines"' && echo "PASS: Baseline export works" || { echo "FAIL: Baseline export failed"; exit 1; }

# Count total lines from summary
TOTAL_LINES=$(echo "$BASELINE_OUTPUT" | grep -o '"count":[0-9]*' | head -1 | grep -o '[0-9]*')
echo "Baseline total lines: $TOTAL_LINES"

# Count branches from summary
TOTAL_BRANCHES=$(echo "$BASELINE_OUTPUT" | grep '"branches"' -A5 | grep '"count"' | head -1 | grep -o '[0-9]*')
echo "Baseline total branches: $TOTAL_BRANCHES"

# Count segments (should have multiple)
SEGMENT_COUNT=$(echo "$BASELINE_OUTPUT" | grep -o '"segments":\[' | wc -l)
echo "Baseline file count (segments arrays): $SEGMENT_COUNT"

# Count branches in output
BRANCH_COUNT=$(echo "$BASELINE_OUTPUT" | grep -o '"branches":\[' | wc -l)
echo "Baseline file count (branches arrays): $BRANCH_COUNT"

echo ""
echo "=== All basic tests passed ==="
echo "Summary:"
echo "  - Total lines in baseline: $TOTAL_LINES"
echo "  - Total branches in baseline: $TOTAL_BRANCHES"

rm -f %t.profdata

echo ""
echo "=== All tests passed ==="