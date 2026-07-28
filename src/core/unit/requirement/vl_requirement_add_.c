#include "../../valve_private.h"

#include <stdlib.h>

int vl_requirement_add_(valve_t *v, const vl_option_t *source,
                        const vl_option_t *target,
                        const valve_verb_t *verb,
                        const valve_verb_t *subverb) {
  if (v->requirements_ &&
      vl_requirement_exists_(v, source, target, verb, subverb))
    return 0;

  if (v->requirement_count_ == v->requirement_cap_) {
    size_t next_cap = v->requirement_cap_ ? v->requirement_cap_ * 2 : 8;
    if (next_cap < v->requirement_cap_)
      return -1;
    size_t bytes;
    if (__builtin_mul_overflow(next_cap, sizeof(valve_requirement_t), &bytes))
      return -1;
    valve_requirement_t *next = realloc(v->requirements_, bytes);
    if (!next)
      return -1;
    v->requirements_ = next;
    v->requirement_cap_ = next_cap;
  }

  v->requirements_[v->requirement_count_++] = (valve_requirement_t){
      .source = source,
      .target = target,
      .verb = verb,
      .subverb = subverb,
  };
  return 0;
}
