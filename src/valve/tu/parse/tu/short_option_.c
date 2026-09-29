#include "../private.h"

int vl_parse_short_option_(valve_t *v, int argc, char **argv, int *index) {

  const char *arg = argv[*index];
  const vl_option_t *opt = vl_option_find_short_(v, arg[1]);
  int flag_index = *index;
  if (!opt) {
    char key[2] = {
      arg[1],
      '\0',
    };

    return vl_error_add_(v, VL_ERROR_UNKNOWN_OPTION, flag_index, key, "unknown option");
  }

  if (!vl_parse_has_style_(opt, VL_OPT_TYPE_SHORT)) {
    return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name, "short type is disabled");
  }

  if (opt->value == VL_OPTION_VALUE_KV) {
    return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name, "kv values require long form");
  }

  if (arg[2] == '\0' && opt->value == VL_OPTION_VALUE_BOOL) {
    /* Bare boolean flag: presence means true, in any assign mode. */
    return vl_parse_bool_value_(v, opt, true, flag_index);
  }

  if (arg[2] == '=') {
    if (v->assign_ != VL_ASSIGN_INLINE) {
      return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name, "short inline form is disabled");
    }

    return vl_parse_assign_value_(v, opt, arg + 3, flag_index);
  }

  if (arg[2] == '\0') {
    if (v->assign_ != VL_ASSIGN_SEPARATE) {
      return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name, "short separate form is disabled");
    }
    if (!vl_parse_consumable_next_value_(argc, argv, *index, opt)) {
      return vl_error_add_(v, VL_ERROR_MISSING_VALUE, flag_index, opt->name, "missing option value");
    }

    ++(*index);

    return vl_parse_assign_value_(v, opt, argv[*index], flag_index);
  }

  return vl_error_add_(v,
                       VL_ERROR_UNEXPECTED_ARGUMENT,
                       flag_index,
                       opt->name,
                       "unexpected characters after short option");
}
