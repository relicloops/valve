#include "../private.h"

#include <stdio.h>

void vl_version_print_default_(const valve_t *v) {
  const char *ver =
      (v && v->program_version_) ? v->program_version_ : PROGRAM_VERSION;
  fputs(ver, stdout);
  fputc('\n', stdout);
}
