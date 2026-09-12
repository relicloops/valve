#include "../../private.h"

bool vl_requirement_exists_(const valve_t *v,
                            const vl_option_t *source,
                            const vl_option_t *target,
                            const valve_verb_t *verb,
                            const valve_verb_t *subverb) {
  for (size_t i = 0; i < v->requirement_count_; ++i) {
    const valve_requirement_t *requirement = &v->requirements_[i];
    /* GCOVR_EXCL_BR_START — partial-match short-circuit */
    if (requirement->verb == verb && requirement->subverb == subverb &&
        requirement->source == source && requirement->target == target)
      return true;
    /* GCOVR_EXCL_BR_STOP */
  }
  return false;
}
