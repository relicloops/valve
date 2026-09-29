#include "../private.h"

int vl_parse_enable_disable_(valve_t *v, const char *arg, int argv_index) {

  const char *ref = nullptr;
  bool enabled = false;
  if (!vl_parse_is_enable_disable_(arg, &ref, &enabled)) {
    return VL_PARSE_CONTINUE;
  }

  const vl_option_t *opt = vl_option_find_toggle_ref_(v, ref);
  if (!opt) {
    return VL_PARSE_CONTINUE;
  }

  if (!vl_parse_has_style_(opt, VL_OPT_TYPE_TOGGLE)) {
    return vl_error_add_(v, VL_ERROR_DISABLED_FORM, argv_index, opt->name, "enable/disable type is disabled");
  }

  if (opt->value != VL_OPTION_VALUE_TOGGLE) {
    return vl_error_add_(v, VL_ERROR_DISABLED_FORM, argv_index, opt->name, "option do not accept toggle value");
  }

  return vl_parse_bool_value_(v, opt, enabled, argv_index);
}
