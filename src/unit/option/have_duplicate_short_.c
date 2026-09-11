#include "../../private.h"

#include <stdio.h>
#include <string.h>

bool options_have_duplicate_short_(const vl_option_t *const *options, size_t count) {

  for (size_t i = 0; i < count; ++i) {
    if (!options[i] || options[i]->short_name == '\0') /* GCOVR_EXCL_BR_LINE: null option continue */
      continue;
    for (size_t j = i + 1; j < count; ++j) {
      if (options[j] && options[j]->short_name == options[i]->short_name)
        return true;
    }
  }
  return false;
}