#include "../../valve_private.h"

#include <stdlib.h>

int vl_conflict_add_(valve_t *v, const vl_option_t *first,
                     const vl_option_t *second, const valve_verb_t *verb,
                     const valve_verb_t *subverb) {
  if (v->conflicts_ &&
      vl_conflict_exists_(v, first, second, verb, subverb))
    return 0;

  if (v->conflict_count_ == v->conflict_cap_) {
    size_t next_cap = v->conflict_cap_ ? v->conflict_cap_ * 2 : 8;
    if (next_cap < v->conflict_cap_)
      return -1;
    size_t bytes;
    if (__builtin_mul_overflow(next_cap, sizeof(valve_conflict_t), &bytes))
      return -1;
    valve_conflict_t *next = realloc(v->conflicts_, bytes);
    if (!next)
      return -1;
    v->conflicts_ = next;
    v->conflict_cap_ = next_cap;
  }

  v->conflicts_[v->conflict_count_++] = (valve_conflict_t){
      .first = first,
      .second = second,
      .verb = verb,
      .subverb = subverb,
  };
  return 0;
}
