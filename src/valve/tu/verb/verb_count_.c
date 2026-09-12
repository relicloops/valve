#include "../../private.h"

size_t verb_count_(const vl_executable_t *settings) {
  size_t n = 0;

  if (!settings || !settings->verbs)
    return 0;

  if (settings->verb_count > 0)
    return settings->verb_count;

  while (settings->verbs[n]) {
    ++n;
  }

  return n;
}