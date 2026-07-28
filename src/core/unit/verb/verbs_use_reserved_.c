#include "../../valve_private.h"

bool verbs_use_reserved_(const vl_verb_t *const *verbs,
                                   const size_t count) {

  for (size_t i = 0; i < count; ++i) {
    if (!verbs[i])
      continue;
    if (vl_reserved_name_(verbs[i]->name) != VL_RESERVED_NONE)
      return true;
  }
  return false;
}