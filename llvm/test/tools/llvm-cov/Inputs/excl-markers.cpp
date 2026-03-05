int main(int argc, char **argv) {
  int x = 0;
  int y = 0;

  // -- single-line exclusion --
  if (argc > 1)
    x = 1;                              // LCOV_EXCL_LINE
  else
    x = 2;

  // LCOV_EXCL_START
  if (argc > 2)
    y = 1;
  else
    y = 2;
  // LCOV_EXCL_STOP

  // -- branch-only single-line exclusion --
  if (argc > 3)                         // LCOV_EXCL_BR_LINE
    x = 10;
  else
    x = 20;

  // LCOV_EXCL_BR_START
  if (argc > 4)
    y = 10;
  else
    y = 20;
  // LCOV_EXCL_BR_STOP

  // -- normal (not excluded) code for comparison --
  if (argc > 5)
    x = 100;
  else
    x = 200;

  return x + y;
}
