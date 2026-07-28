#include "../../valve_private.h"

bool verbs_have_invalid_(const vl_verb_t *const *verbs,
const size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (!verbs[i] || !verb_name_valid_(verbs[i]->name))
      return true;
  }

  return false;
}