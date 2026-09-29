#include "../private.h"

int vl_parse_check_int_bounds_(valve_t *v, const vl_option_t *opt, int64_t value, int argv_index) {

  /* A duration is bounded in seconds, so reporting its bound as an "integer"
     describes the storage rather than what the user typed. */
  const bool duration = vl_parse_effective_scalar_value_(opt) == VL_OPTION_VALUE_TIME;
  if (opt->has_int_min && value < opt->int_min) {
    return vl_error_add_(v,
                         VL_ERROR_INVALID_VALUE,
                         argv_index,
                         opt->name,
                         duration ? "duration value is below minimum" : "integer value is below minimum") == 0
             ? 1
             : -1;
  }

  if (opt->has_int_max && value > opt->int_max) {
    return vl_error_add_(v,
                         VL_ERROR_INVALID_VALUE,
                         argv_index,
                         opt->name,
                         duration ? "duration value is above maximum" : "integer value is above maximum") == 0
             ? 1
             : -1;
  }

  return 0;
}
