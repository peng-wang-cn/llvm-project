// Test source for LCOV_EXCL marker export tests.
// Each function exercises a specific marker type.

int branch_a, branch_b;

// --- LCOV_EXCL_LINE ---
// Line 7: a normal branch (should appear in export)
int test_excl_line(int x) {
  if (x)       // line 10: normal branch
    branch_a = 1;
  else
    branch_a = 0;
  return branch_a; // line 13: LCOV_EXCL_LINE (single-line exclusion)
}

// --- LCOV_EXCL_START / STOP ---
int test_excl_region(int y) {
  int z = 0;
  // LCOV_EXCL_START
  if (y)       // line 21: inside exclusion region
    z = 1;
  else         // line 23: inside exclusion region
    z = 0;
  // LCOV_EXCL_STOP
  if (z)       // line 27: outside exclusion region, normal branch
    branch_b = 1;
  else
    branch_b = 0;
  return branch_b;
}

// --- LCOV_EXCL_BR_LINE ---
int test_excl_br_line(int w) {
  if (w)       // line 35: branch-only exclusion (line coverage still counted)
    branch_a = 2;
  else
    branch_a = 3;
  return branch_a; // line 39: normal
}

int main() {
  test_excl_line(1);
  test_excl_region(1);
  test_excl_br_line(1);
  return 0;
}
