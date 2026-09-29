#include "../../private.h"

#include <string.h>

bool option_bool_literal_(const char *raw, bool *out) {
  if (strcmp(raw, "true") == 0) {
    *out = true;
    return true;
  }

  if (strcmp(raw, "false") == 0) {
    *out = false;
    return true;
  }

  return false;
}
