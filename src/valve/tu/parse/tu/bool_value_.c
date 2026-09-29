#include "../private.h"

#include <string.h>

int vl_parse_bool_value_(valve_t *v, const vl_option_t *opt, bool enabled,
                         int argv_index) {

  vl_value_t value = {
      .kind = VL_VALUE_BOOL,
      .raw = strdup(enabled ? "true" : "false"),
      .as.boolean = enabled,
  };

  if (!value.raw)
    return -1;

  if (vl_result_set_(v, opt, &value, argv_index) != 0) {
    vl_value_clear(&value);
    return -1;
  }

  return 0;
}
