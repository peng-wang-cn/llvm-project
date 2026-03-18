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
