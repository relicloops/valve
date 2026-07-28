#include "../../valve_private.h"

size_t verb_table_count_(const vl_verb_t *const *verbs, size_t verb_count) {
  size_t n = 0;

  if (!verbs)
    return 0;

  if (verb_count > 0)
    return verb_count;

  while (verbs[n]) {
    ++n;
  }

  return n;
}
