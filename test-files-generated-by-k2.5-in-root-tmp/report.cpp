// Test file for report with LCOV exclusion markers
// /tmp/report.cpp (in the object file it expects this path)

void foo(bool b) {
  if (b) {
    return;  // LCOV_EXCL_LINE
  }
  return;
}

void bar() {
    // LCOV_EXCL_START
    int x = 0;
    // LCOV_EXCL_STOP
}

void func() {
    // LCOV_EXCL_LINE
}

int main() {
  foo(true);
  bar();
  return 0;
}
