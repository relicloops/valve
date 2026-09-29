#include "../private.h"

/* Post-token checks that only make sense once every option has been seen:
 * required options, directed requirements, and conflicts. */
int vl_parse_finish_(valve_t *v) {

  if (v->error_count_) {
    return -1;
  }

  if (vl_required_check_(v) != 0) {
    (void)vl_error_add_(v, VL_ERROR_OUT_OF_MEMORY, 0, nullptr, "out of memory");

    return -1;
  }

  if (vl_requirements_check_(v) != 0) {
    (void)vl_error_add_(v, VL_ERROR_OUT_OF_MEMORY, 0, nullptr, "out of memory");

    return -1;
  }

  if (vl_conflicts_check_(v) != 0) {
    (void)vl_error_add_(v, VL_ERROR_OUT_OF_MEMORY, 0, nullptr, "out of memory");

    return -1;
  }

  return v->error_count_ ? -1 : 0;
}
