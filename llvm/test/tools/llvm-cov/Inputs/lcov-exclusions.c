// Test file for LCOV exclusion markers in llvm-cov export
// This file tests all major LCOV exclusion marker types

// LCOV_EXCL_LINE - single line exclusion
int excludedLine() { // LCOV_EXCL_LINE
  return 1;          // LCOV_EXCL_LINE
}                    // LCOV_EXCL_LINE

// LCOV_EXCL_START / LCOV_EXCL_STOP - block exclusion
int excludedBlock() {
  int x = 0; // LCOV_EXCL_START
  x = 1;
  x = 2;     // LCOV_EXCL_STOP
  return x;
}

// Branch exclusion LCOV_EXCL_BR_LINE - lines excluded only from branch coverage
int branchExcludedLine() {
  int a = 5;      // LCOV_EXCL_BR_LINE
  int b = 10;     // LCOV_EXCL_BR_LINE
  if (a > b) {    // LCOV_EXCL_BR_LINE
    return a;     // LCOV_EXCL_BR_LINE
  }               // LCOV_EXCL_BR_LINE
  return b;       // LCOV_EXCL_BR_LINE
}

// Branch exclusion block LCOV_EXCL_BR_START / LCOV_EXCL_BR_STOP
int branchExcludedBlock() { // LCOV_EXCL_BR_START
  int x = 0;
  if (x > 0) {
    return 1;
  }
  return 0;       // LCOV_EXCL_BR_STOP
}                 // LCOV_EXCL_BR_STOP

// Normal function (not excluded)
int normalFunction() {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += i;
  }
  return sum;
}

int main() {
  // Call all functions
  int r1 = excludedLine();
  int r2 = excludedBlock();
  int r3 = branchExcludedLine();
  int r4 = branchExcludedBlock();
  int r5 = normalFunction();
  return r1 + r2 + r3 + r4 + r5;
}