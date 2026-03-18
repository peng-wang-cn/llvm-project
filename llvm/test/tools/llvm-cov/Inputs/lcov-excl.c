// Test file for LCOV exclusion markers

void simple_loops() {
  int i;
  for (i = 0; i < 100; ++i) {   // LCOV_EXCL_LINE
  }
  while (i > 0)
    i--;
  do {} while (i++ < 75);
}

void conditionals() {
  // LCOV_EXCL_START
  for (int i = 0; i < 100; ++i) {
    if (i % 2) {
      if (i) {}
    } else if (i % 3) {
      if (i) {}
    } else {
      if (i) {}
    }
    if (1 && i) {}
    if (0 || i) {}
  }
  // LCOV_EXCL_STOP
}

void early_exits() {
  int i = 0;

  if (i) {}

  while (i < 100) {
    i++;
    if (i > 50)
      break;
    if (i % 2)
      continue;
  }

  if (i) {}

  do {
    if (i > 75)
      return;
    else
      i++;
  } while (i < 100);

  if (i) {}

}

void jumps() {
  int i;

  for (i = 0; i < 2; ++i) {
    goto outofloop;
    if (i) {}
  }

outofloop:
  if (i) {}

  goto loop1;

  while (i) {
  loop1:
    if (i) {}
  }

  goto loop2;
first:
second:
third:
  i++;
  if (i < 3)
    goto loop2;

  while (i < 3) {
  loop2:
    switch (i) {
    case 0:
      goto first;
    case 1:
      goto second;
    case 2:
      goto third;
    }
  }

  for (i = 0; i < 10; ++i) {
    goto withinloop;
    if (i) {}
  withinloop:
    if (i) {}
  }

}

void switches() {
  static int weights[] = {1, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5};

  switch (weights[0]) {
  default:
    break;
  }

  for (int i = 0, len = sizeof(weights) / sizeof(weights[0]); i < len; ++i) {
    switch (i[weights]) {
    case 1:
      if (i) {}  // LCOV_EXCL_BR_LINE
    case 2:
      if (i) {}
      break;
    case 3:
      if (i) {}
      continue;
    case 4:
      if (i) {}
      switch (i) {
      case 6 ... 9:
        if (i) {}
        continue;
      }

    default:
      if (i == len - 1)
        return;
    }
  }

  if (weights[0]) {}

}

void big_switch() {
  // LCOV_EXCL_START
  for (int i = 0; i < 32; ++i) {
    switch (1 << i) {
    case (1 << 0):
      if (i) {}
    case (1 << 1):
      if (i) {}
      break;
    case (1 << 2) ... (1 << 12):
      if (i) {}
      break;

    case (1 << 13):
      if (i) {}
      break;
    case (1 << 14) ... (1 << 28):
      if (i) {}
      break;

    case (1 << 29) ... ((1 << 29) + 1):
      if (i) {}
      break;
    default:
      if (i) {}
      break;
    }
  }
  // LCOV_EXCL_STOP
}

void boolean_operators() {
  int v;
  for (int i = 0; i < 100; ++i) {
    v = i % 3 || i;
    v = i % 3 && i;
    v = i % 3 || i % 2 || i;
    v = i % 2 && i % 3 && i;
  }
}

void boolop_loops() {
  int i = 100;

  while (i && i > 50)
    i--;

  while ((i % 2) || (i > 0))
    i--;

  for (i = 100; i && i > 50; --i);
  for (; (i % 2) || (i > 0); --i);
}

void conditional_operator() {
  int i = 100;

  int j = i < 50 ? i : 1;

  int k = i ?: 0;

}

void do_fallthrough() {
  for (int i = 0; i < 10; ++i) {
    int j = 0;
    do {
      if (i < 8) break;
      j++;
    } while (j < 2);
  }
}

static void static_func() {
  // LCOV_EXCL_START
  for (int i = 0; i < 10; ++i) {
  }
  // LCOV_EXCL_STOP
}











int main(int argc, const char *argv[]) {
  simple_loops();
  conditionals();
  early_exits();
  jumps();
  switches();
  big_switch();
  boolean_operators();
  boolop_loops();
  conditional_operator();
  do_fallthrough();
  static_func();
  (void)0;
  (void)0;
  return 0;
}
