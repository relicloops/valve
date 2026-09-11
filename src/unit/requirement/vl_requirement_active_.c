#include "../../private.h"

bool vl_requirement_active_(const valve_t *v,
                            const valve_requirement_t *requirement) {
  if (!v || !requirement)
    return false;
  if (requirement->verb && requirement->verb != v->active_verb_)
    return false;
  if (requirement->subverb &&
      requirement->subverb != v->active_subverb_)
    return false;
  return true;
}
