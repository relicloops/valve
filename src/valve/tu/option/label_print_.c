#include "../../private.h"

#include <stdio.h>
#include <stdlib.h>

void option_label_print_(const vl_option_t *option, bool comma) {
  char *label = option_label_(option);
  if (!label) {
    printf("%s--%s", comma ? ", " : "", option->name);
    return;
  }

  printf("%s%s", comma ? ", " : "", label);
  free(label);
}
