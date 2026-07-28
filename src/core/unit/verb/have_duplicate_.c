#include "../../valve_private.h"

#include <string.h>

bool verbs_have_duplicate_(const vl_verb_t *const *verbs,
const size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (!verbs[i] || !verbs[i]->name) /* GCOVR_EXCL_BR_LINE: null name continue */
      continue;
    for (size_t j = i + 1; j < count; ++j) {
      /* GCOVR_EXCL_BR_START — null slot short-circuit */
      if (verbs[j] && verbs[j]->name &&
          strcmp(verbs[i]->name, verbs[j]->name) == 0)
        return true;
      /* GCOVR_EXCL_BR_STOP */
    }
  }
  return false;
}