#include "../../valve_private.h"

int vl_conflicts_check_(valve_t *v) {
  for (size_t i = 0; i < v->conflict_count_; ++i) {
    const valve_conflict_t *conflict = &v->conflicts_[i];
    if (!vl_conflict_active_(v, conflict))
      continue;

    const vl_result_t *first =
        vl_result_find_(v, conflict->first->name);
    const vl_result_t *second =
        vl_result_find_(v, conflict->second->name);
    if (!first || !second)
      continue;

    if (first->argv_index <= second->argv_index) {
      if (vl_conflict_error_(v, conflict->second, second,
                             conflict->first) != 0)
        return -1;
    } else if (vl_conflict_error_(v, conflict->first, first,
                                  conflict->second) != 0) {
      return -1;
    }
  }
  return 0;
}
