#include "../private.h"

#include <stdio.h>
#include <string.h>

int vl_parse_scalar_value_(valve_t *v, const vl_option_t *opt, const char *raw, int argv_index) {

  vl_value_t value = {
    .kind = VL_VALUE_STRING,
    .raw = strdup(raw),
  };
  vl_option_value_t vk = vl_parse_effective_scalar_value_(opt);
  int64_t integer = 0;
  double number = 0.0;

  if (!value.raw) {
    return -1;
  }

  if (vk == VL_OPTION_VALUE_INT) {
    if (!vl_parse_int_literal_(raw, &integer)) {
      vl_value_clear(&value);

      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name, "expected integer value");
    }

    int bounds_rc = vl_parse_check_int_bounds_(v, opt, integer, argv_index);
    if (bounds_rc != 0) {
      vl_value_clear(&value);

      return bounds_rc < 0 ? -1 : 0;
    }

    value.kind = VL_VALUE_INT;
    value.as.integer = integer;
  } else if (vk == VL_OPTION_VALUE_DOUBLE) {
    if (!vl_parse_double_literal_(raw, &number)) {
      vl_value_clear(&value);

      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name, "expected double value");
    }

    value.kind = VL_VALUE_DOUBLE;
    value.as.number = number;
  } else if (vk == VL_OPTION_VALUE_NUMBER) {
    if (vl_parse_token_has_double_mark_(raw) && vl_parse_double_literal_(raw, &number)) {
      value.kind = VL_VALUE_DOUBLE;
      value.as.number = number;
    } else if (vl_parse_int_literal_(raw, &integer)) {
      int bounds_rc = vl_parse_check_int_bounds_(v, opt, integer, argv_index);
      if (bounds_rc != 0) {
        vl_value_clear(&value);

        return bounds_rc < 0 ? -1 : 0;
      }

      value.kind = VL_VALUE_INT;
      value.as.integer = integer;
    } else if (vl_parse_double_literal_(raw, &number)) {
      value.kind = VL_VALUE_DOUBLE;
      value.as.number = number;
    } else {
      vl_value_clear(&value);

      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name, "expected numeric value");
    }
  } else if (vk == VL_OPTION_VALUE_TIME) {
    option_duration_status_t status = option_duration_parse_(raw, &integer);
    if (status != OPTION_DURATION_OK) {
      char message[192];

      snprintf(message, sizeof message, "%s: '%s'", option_duration_message_(status), raw);
      vl_value_clear(&value);

      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name, message);
    }

    /* The literal is now seconds, so has_int_min / has_int_max bound the
       converted count rather than whatever the user typed. */
    int bounds_rc = vl_parse_check_int_bounds_(v, opt, integer, argv_index);
    if (bounds_rc != 0) {
      vl_value_clear(&value);

      return bounds_rc < 0 ? -1 : 0;
    }

    value.kind = VL_VALUE_INT;
    value.as.integer = integer;
  } else if (vk == VL_OPTION_VALUE_BOOL) {
    bool boolean = false;
    if (!option_bool_literal_(raw, &boolean)) {
      vl_value_clear(&value);

      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name, "expected boolean value");
    }

    value.kind = VL_VALUE_BOOL;
    value.as.boolean = boolean;
  } else if (vk == VL_OPTION_VALUE_AUTO) {
    option_scalar_status_t status = option_scalar_auto_(&value);
    if (status != OPTION_SCALAR_OK) {
      vl_value_clear(&value);

      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name, option_scalar_message_(status));
    }
  }

  if (vl_result_set_(v, opt, &value, argv_index) != 0) {
    vl_value_clear(&value);

    return -1;
  }

  return 0;
}
