#include "../../valve_private.h"

int vl_requirements_check_(valve_t *v) {
  for (size_t i = 0; i < v->requirement_count_; ++i) {
    const valve_requirement_t *requirement = &v->requirements_[i];
    if (!vl_requirement_active_(v, requirement))
      continue;

    const vl_result_t *source =
        vl_result_find_(v, requirement->source->name);
    if (!source ||
        vl_result_find_(v, requirement->target->name))
      continue;

    if (vl_requirement_error_(v, requirement->source, source,
                              requirement->target) != 0)
      return -1;
  }
  return 0;
}
