#include <stdio.h>
#include <string.h>

#include "tcp.h"
#include "ds.h"
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

/*
** ./bin/modric # no params, runs json_demo()
** ./bin/modric -alvarez # run testing code
** ./bin/modric -e2j file.edn  # convert edn to json and pprint
** ./bin/modric -ppj file.json # pprint json
**
**  # rocksdb playin'
**    # insert doc
**  ./bin/modric -db path-to-db -key string-key-for-json -json path-to-json-file
**    # print doc
**  ./bin/modric -db path-to-db -key string-key-for-json
* */
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
      ds_test();
      break;
    } else {
      usage(argv[0]);
      break;
    }
  }

  return 0;
}
