#include "../../private.h"

bool options_use_reserved_(const vl_option_t *const *options, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (!options[i])
      continue;
    if (vl_reserved_name_(options[i]->name) != VL_RESERVED_NONE)
      return true;
    if (options[i]->short_name != '\0' &&
        vl_reserved_short_(options[i]->short_name) != VL_RESERVED_NONE)
      return true;
  }
  return false;
}