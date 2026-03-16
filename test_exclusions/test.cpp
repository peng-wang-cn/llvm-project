// Test file for LCOV exclusion markers
// LCOV_EXCL_LINE
int excludedLine() { // LCOV_EXCL_LINE
  return 1;          // LCOV_EXCL_LINE
}                    // LCOV_EXCL_LINE

// LCOV_EXCL_START / LCOV_EXCL_STOP
int excludedBlock() {
  int x = 0; // LCOV_EXCL_START
  x = 1;
  x = 2;     // LCOV_EXCL_STOP
  return x;
}

// Branch exclusion markers
int branchExcluded() {
  int a = 5;      // LCOV_EXCL_BR_LINE
  int b = 10;     // LCOV_EXCL_BR_LINE
  if (a > b) {    // LCOV_EXCL_BR_LINE
    return a;     // LCOV_EXCL_BR_LINE
  }               // LCOV_EXCL_BR_LINE
  return b;       // LCOV_EXCL_BR_LINE
}

// Branch exclusion block
int branchExcludedBlock() { // LCOV_EXCL_BR_START
  int x = 0;
  if (x > 0) {   // LCOV_EXCL_BR_START
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
  excludedBlock();
  branchExcluded();
  branchExcludedBlock();
  normalFunction();
  return 0;
}
