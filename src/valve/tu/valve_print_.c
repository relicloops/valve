#include "../private.h"

#include <stdio.h>

void vl_valve_print_default_(const valve_t *v) {
  (void)v;
  fputs(vl_version_string(), stdout);
  fputc('\n', stdout);
}
