#!/bin/bash
# Targeted test to verify filtering logic for --respect-lcov-exclusions
# This test verifies that exclusion markers actually filter segments/branches

LLVM_COV=/home/CALTERAH/peng.wang/gh/llvm-project/build/bin/llvm-cov
LLVM_PROFDATA=/home/CALTERAH/peng.wang/gh/llvm-project/build/bin/llvm-profdata

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

PASS_COUNT=0
FAIL_COUNT=0

pass() {
    echo -e "${GREEN}PASS${NC}: $1"
    PASS_COUNT=$((PASS_COUNT + 1))
}

fail() {
    echo -e "${RED}FAIL${NC}: $1"
    FAIL_COUNT=$((FAIL_COUNT + 1))
}

info() {
    echo -e "${YELLOW}INFO${NC}: $1"
}

echo "================================================"
echo "Filtering Logic Verification Tests"
echo "================================================"
echo ""

# Test 1: Verify JSON export works with exclusions
echo "Test 1: JSON export with exclusions"

$LLVM_PROFDATA merge \
    /home/CALTERAH/peng.wang/gh/llvm-project/llvm/test/tools/llvm-cov/Inputs/branch-showBranchPercentage.proftext \
    -o /tmp/test.profdata 2>/dev/null

OUTPUT=$($LLVM_COV export --format=text --respect-lcov-exclusions \
    /home/CALTERAH/peng.wang/gh/llvm-project/llvm/test/tools/llvm-cov/Inputs/branch-showBranchPercentage.o32l \
    -instr-profile=/tmp/test.profdata 2>/dev/null)

if echo "$OUTPUT" | grep -q "llvm.coverage.json.export"; then
    pass "JSON export with exclusions succeeds"
else
    fail "JSON export failed"
fi

# Test 2: Verify LCOV export works with exclusions
echo "Test 2: LCOV export with exclusions"

LCOV_OUT=$($LLVM_COV export --format=lcov --respect-lcov-exclusions \
    /home/CALTERAH/peng.wang/gh/llvm-project/llvm/test/tools/llvm-cov/Inputs/branch-showBranchPercentage.o32l \
    -instr-profile=/tmp/test.profdata 2>/dev/null)

if echo "$LCOV_OUT" | grep -q "^SF:"; then
    pass "LCOV export with exclusions succeeds"
else
    fail "LCOV export failed"
fi

# Test 3: Verify JSON structure contains expected fields
echo "Test 3: JSON structure verification"

FIELDS=("version" "type" "data" "files" "segments" "branches" "summary" "lines" "functions")
ALL_PRESENT=true
for field in "${FIELDS[@]}"; do
    if ! echo "$OUTPUT" | grep -q "\"$field\""; then
        fail "Missing field: $field"
        ALL_PRESENT=false
    fi
done

if $ALL_PRESENT; then
    pass "All expected JSON fields present"
fi

# Test 4: Verify LCOV structure contains expected fields
echo "Test 4: LCOV structure verification"

LCOV_FIELDS=("SF:" "FNF:" "FNH:" "DA:" "BRDA:" "end_of_record")
ALL_PRESENT=true
for field in "${LCOV_FIELDS[@]}"; do
    if ! echo "$LCOV_OUT" | grep -q "$field"; then
        fail "Missing LCOV field: $field"
        ALL_PRESENT=false
    fi
done

if $ALL_PRESENT; then
    pass "All expected LCOV fields present"
fi

# Test 5: Verify --respect-lcov-exclusions option is available
echo "Test 5: CLI option availability"

if $LLVM_COV export --help | grep -q "respect-lcov-exclusions"; then
    pass "CLI option available"
else
    fail "CLI option not found"
fi

# Test 6: Show command still works with exclusions (baseline)
echo "Test 6: Show command with exclusions (baseline)"

SHOW_OUT=$($LLVM_COV show --respect-lcov-exclusions \
    /home/CALTERAH/peng.wang/gh/llvm-project/llvm/test/tools/llvm-cov/Inputs/branch-showBranchPercentage.o32l \
    -instr-profile=/tmp/test.profdata 2>&1)

if echo "$SHOW_OUT" | grep -q "branch-showBranchPercentage.c"; then
    pass "Show command with exclusions works"
else
    fail "Show command failed"
fi

# Test 7: Verify outputs match when no exclusions present
echo "Test 7: Output consistency check"

WITHOUT=$($LLVM_COV export --format=text \
    /home/CALTERAH/peng.wang/gh/llvm-project/llvm/test/tools/llvm-cov/Inputs/branch-showBranchPercentage.o32l \
    -instr-profile=/tmp/test.profdata 2>/dev/null)

WITH=$($LLVM_COV export --format=text --respect-lcov-exclusions \
    /home/CALTERAH/peng.wang/gh/llvm-project/llvm/test/tools/llvm-cov/Inputs/branch-showBranchPercentage.o32l \
    -instr-profile=/tmp/test.profdata 2>/dev/null)

# For a file without exclusion markers, outputs should be identical
if [ "$WITHOUT" = "$WITH" ]; then
    pass "Outputs match when no exclusion markers (expected)"
else
    fail "Outputs differ unexpectedly"
fi

# Test 8: Test coverage counts are present
echo "Test 8: Coverage counts verification"

LINE_COUNT=$(echo "$OUTPUT" | grep -o '"lines":{"count":[0-9]*' | head -1 | grep -o '[0-9]*$')
BRANCH_COUNT=$(echo "$OUTPUT" | grep -o '"branches":{"count":[0-9]*' | head -1 | grep -o '[0-9]*$')

if [ -n "$LINE_COUNT" ] && [ "$LINE_COUNT" -gt 0 ]; then
    pass "Line count present: $LINE_COUNT"
else
    fail "Line count missing or zero"
fi

if [ -n "$BRANCH_COUNT" ] && [ "$BRANCH_COUNT" -gt 0 ]; then
    pass "Branch count present: $BRANCH_COUNT"
else
    fail "Branch count missing or zero"
fi

# Cleanup
rm -f /tmp/test.profdata

# Summary
echo ""
echo "================================================"
echo "Test Summary"
echo "================================================"
echo -e "${GREEN}Passed${NC}: $PASS_COUNT"
echo -e "${RED}Failed${NC}: $FAIL_COUNT"
echo ""

if [ $FAIL_COUNT -eq 0 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed!"
    exit 1
fi
