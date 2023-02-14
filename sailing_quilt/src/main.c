#include <stdio.h>
#include <string.h>

#include "test.h"
#include "modriclib.h"

static void usage(const char *prog) {
  fprintf(stderr,
          "sailing-quilt"
          "Usage: %s OPTIONS\n"
          "  -client  - send and receive\n"
          "  -server  - receive and send\n"
          "  -test    - run the latest random shniz\n",
          prog);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  // Parse command-line flags
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-test") == 0) {
      g_test(argc, argv);
      break;
    } else if (strcmp(argv[i], "-demo") == 0) {
      printf("Run some experimental smack!\n");
      break;
    } else {
      usage(argv[0]);
      break;
    }
  }

  return 0;
}
