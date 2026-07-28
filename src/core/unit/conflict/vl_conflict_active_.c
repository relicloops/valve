#include "../../valve_private.h"

bool vl_conflict_active_(const valve_t *v,
                         const valve_conflict_t *conflict) {
  if (!v || !conflict)
    return false;
  if (conflict->verb && conflict->verb != v->active_verb_)
    return false;
  if (conflict->subverb && conflict->subverb != v->active_subverb_)
    return false;
  return true;
}
