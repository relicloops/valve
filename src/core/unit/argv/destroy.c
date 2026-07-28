#include "../../valve_private.h"

#include <stdlib.h>

void vl_argv_destroy(int argc, char **argv) {
  if (!argv)
    return;

  for (int i = 0; i < argc; ++i) {
    free(argv[i]);
  }

  free(argv);
}
