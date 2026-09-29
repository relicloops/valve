#include "../private.h"

#include <string.h>

int vl_parse_array_value_(valve_t *v, const vl_option_t *opt, const char *raw, int argv_index) {

  vl_value_t value = {
    .kind = VL_VALUE_ARRAY,
    .raw = strdup(raw),
  };
  if (!value.raw) {
    return -1;
  }

  if (*raw == '\0') {
    vl_value_clear(&value);

    return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name, "expected non-empty array value");
  }

  const char *cursor = raw;
  while (*cursor) {
    vl_value_t element = {0};
    const char *message = nullptr;

    int rc = vl_parse_array_element_(&cursor, &element, &message);
    if (rc == -2) {
      vl_value_clear(&element);
      vl_value_clear(&value);

      return -1;
    }

    if (rc != 0) {
      vl_value_clear(&element);
      vl_value_clear(&value);

      return vl_error_add_(v,
                           VL_ERROR_INVALID_VALUE,
                           argv_index,
                           opt->name,
                           message ? message : "expected comma-separated values");
    }

    if (vl_value_array_push_(&value.as.array, &element) != 0) {
      vl_value_clear(&element);
      vl_value_clear(&value);

      return -1;
    }

    if (*cursor == ',') {
      ++cursor;

      if (*cursor == '\0') {
        vl_value_clear(&value);

        return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name, "trailing comma in array value");
      }
    } else if (*cursor != '\0') {
      vl_value_clear(&value);

      return vl_error_add_(v, VL_ERROR_INVALID_VALUE, argv_index, opt->name, "unexpected character in array value");
    }
  }

  if (vl_result_set_(v, opt, &value, argv_index) != 0) {
    vl_value_clear(&value);

    return -1;
  }

  return 0;
}
