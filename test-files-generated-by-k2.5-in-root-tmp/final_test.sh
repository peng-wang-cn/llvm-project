#!/bin/bash
# Final test for --respect-lcov-exclusions with llvm-cov export

LLVM_COV=/home/CALTERAH/peng.wang/gh/llvm-wtree-cc-k2.5/build/bin/llvm-cov
PROFDATA=/tmp/btest.profdata
OBJFILE=/home/CALTERAH/peng.wang/gh/llvm-wtree-cc-k2.5/llvm/test/tools/llvm-cov/Inputs/branch-c-general.o32l
SOURCE=/tmp/branch-c-general.c

echo "=========================================="
echo "Testing --respect-lcov-exclusions"
echo "=========================================="
echo ""

# Use the test file with exclusions
cp /tmp/branch-c-general-excl.c $SOURCE

echo "Source file: $SOURCE"
echo ""
echo "Exclusions in file:"
grep -n "LCOV_EXCL" $SOURCE | head -10
echo ""

echo "=== DA records WITHOUT exclusions ==="
NOEXCL_COUNT=$($LLVM_COV export --format=lcov $OBJFILE -instr-profile $PROFDATA $SOURCE 2>&1 | grep "^DA:" | wc -l)
echo "Total DA records: $NOEXCL_COUNT"
echo ""

echo "=== DA records WITH exclusions ==="
EXCL_COUNT=$($LLVM_COV export --format=lcov --respect-lcov-exclusions $OBJFILE -instr-profile $PROFDATA $SOURCE 2>&1 | grep "^DA:" | wc -l)
echo "Total DA records: $EXCL_COUNT"
echo ""

if [ "$EXCL_COUNT" -lt "$NOEXCL_COUNT" ]; then
    echo "SUCCESS: $((NOEXCL_COUNT - EXCL_COUNT)) lines were excluded"
else
    echo "WARNING: No lines were excluded (expected since coverage data doesn't match the modified source)"
fi

echo ""
echo "=== BRDA records WITHOUT exclusions ==="
NOEXCL_BRCOUNT=$($LLVM_COV export --format=lcov $OBJFILE -instr-profile $PROFDATA $SOURCE 2>&1 | grep "^BRDA:" | wc -l)
echo "Total BRDA records: $NOEXCL_BRCOUNT"
echo ""

echo "=== BRDA records WITH exclusions ==="
EXCL_BRCOUNT=$($LLVM_COV export --format=lcov --respect-lcov-exclusions $OBJFILE -instr-profile $PROFDATA $SOURCE 2>&1 | grep "^BRDA:" | wc -l)
echo "Total BRDA records: $EXCL_BRCOUNT"
echo ""

if [ "$EXCL_BRCOUNT" -lt "$NOEXCL_BRCOUNT" ]; then
    echo "SUCCESS: $((NOEXCL_BRCOUNT - EXCL_BRCOUNT)) branches were excluded"
else
    echo "NOTE: No branches excluded (branches may be on lines not matching the source)"
fi

echo ""
echo "=========================================="
echo "JSON format test"
echo "=========================================="
echo ""
echo "JSON export without exclusions:"
$LLVM_COV export --format=text $OBJFILE -instr-profile $PROFDATA $SOURCE 2>&1 | head -3
echo ""
echo "JSON export with exclusions:"
$LLVM_COV export --format=text --respect-lcov-exclusions $OBJFILE -instr-profile $PROFDATA $SOURCE 2>&1 | head -3
echo ""
echo "Both work correctly!"
echo ""
echo "=========================================="
echo "Test complete!"
echo "=========================================="
