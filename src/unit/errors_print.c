#include "../private.h"

#include <stdio.h>

static void print_one_(const vl_error_t *err, void *userdata) {
  FILE *stream = userdata;

  fprintf(stream, "error argv[%d]", err->argv_index);
  if (err->key)
    fprintf(stream, " [%s]", err->key);
  fprintf(stream, ": %s\n", err->message ? err->message : "parse error");
}

void vl_errors_print(const valve_t *v, FILE *stream) {
  if (!stream)
    stream = stderr;
  vl_errors_foreach(v, print_one_, stream);
}
