#include <stdio.h>
#include <string.h>

#include "tcp.h"
#include "ds.h"
#include "hlist.h"
#include "modriclib.h"

static void usage(const char *prog) {
  fprintf(stderr,
          "Borimir "
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
    if (strcmp(argv[i], "-client") == 0) {
      tcp_run_client();
      break;
    } else if (strcmp(argv[i], "-server") == 0) {
      tcp_run_server();
      break;
    } else if (strcmp(argv[i], "-test") == 0) {
      h_test();
      break;
    } else {
      usage(argv[0]);
      break;
    }
  }

  return 0;
}
