#include "../../private.h"

#include <stdlib.h>
#include <string.h>

bool options_have_duplicate_long_(const vl_option_t *const *options, size_t count) {

  for (size_t i = 0; i < count; ++i) {
    if (!options[i] || !options[i]->name) /* GCOVR_EXCL_BR_LINE: null name continue */
      continue;
    for (size_t j = i + 1; j < count; ++j) {
      if (options[j] && options[j]->name &&
          strcmp(options[i]->name, options[j]->name) == 0)
        return true;
    }
  }
  return false;
}