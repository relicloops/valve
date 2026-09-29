#include "../private.h"

int vl_parse_assign_value_(valve_t *v, const vl_option_t *opt, const char *raw, int argv_index) {

  if (opt->value == VL_OPTION_VALUE_KV) {
    return vl_parse_kv_value_(v, opt, raw, argv_index);
  }

  if (opt->value == VL_OPTION_VALUE_ARRAY) {
    return vl_parse_array_value_(v, opt, raw, argv_index);
  }

  return vl_parse_scalar_value_(v, opt, raw, argv_index);
}
