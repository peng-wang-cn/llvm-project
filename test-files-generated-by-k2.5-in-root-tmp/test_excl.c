// Test file for LCOV exclusion markers
#include <stdio.h>

int main(int argc, char *argv[]) {
    int x = 0;

    // LCOV_EXCL_LINE
    x = 1;

    if (argc > 1) {
        x = 2;
    }

    // LCOV_EXCL_START
    x = 3;
    x = 4;
    // LCOV_EXCL_STOP

    if (argc > 2) { // LCOV_EXCL_BR_LINE
        x = 5;
    }

    // LCOV_EXCL_START
    if (argc > 3) {
        x = 6;
    }
    // LCOV_EXCL_STOP

    // LCOV_EXCL_BR_START
    if (argc > 4) {
        x = 7;
    }
    // LCOV_EXCL_BR_STOP

    return x;
}
