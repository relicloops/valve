#include "../../private.h"

bool vl_conflict_exists_(const valve_t *v, const vl_option_t *first,
                         const vl_option_t *second,
                         const valve_verb_t *verb,
                         const valve_verb_t *subverb) {
  for (size_t i = 0; i < v->conflict_count_; ++i) {
    const valve_conflict_t *conflict = &v->conflicts_[i];
    if (conflict->verb != verb || conflict->subverb != subverb) /* GCOVR_EXCL_BR_LINE: scope continue short-circuit */
      continue;
    if ((conflict->first == first && conflict->second == second) ||
        (conflict->first == second && conflict->second == first))
      return true;
  }
  return false;
}
