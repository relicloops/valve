#include "../../private.h"

bool option_in_(const vl_option_t *option,
                const vl_option_t *const *options, size_t count) {
  if (!option || !options)
    return false;

  for (size_t i = 0; i < count; ++i) {
    if (options[i] == option)
      return true;
  }
  return false;
}
