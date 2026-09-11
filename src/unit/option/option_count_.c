#include "../../private.h"

size_t option_count_(const vl_option_t *const *options, size_t option_count) {

  size_t n = 0;

  if (!options)
    return 0;

  if (option_count > 0)
    return option_count;

  while (options[n]) {
    ++n;
  }

  return n;
}