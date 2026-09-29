#include "../private.h"

#include <stdlib.h>
#include <string.h>

int vl_parse_long_option_(valve_t *v, int argc, char **argv, int *index) {
  const char *arg = argv[*index];
  const char *name = arg + 2;
  const char *eq = strchr(name, '=');
  const vl_option_t *opt = nullptr;
  int flag_index = *index;

  if (eq) {
    opt = vl_option_find_n_(v, name, (size_t)(eq - name));
    if (!opt) {
      char *tmp = strndup(name, (size_t)(eq - name));
      int rc = 0;
      if (!tmp)
        return -1;
      rc = vl_error_add_(v, VL_ERROR_UNKNOWN_OPTION, flag_index, tmp,
                         "unknown option");
      free(tmp);
      return rc;
    }
    if (opt->value == VL_OPTION_VALUE_COMMAND) {
      return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name,
                           "command is given after --");
    }
    if (!vl_parse_has_style_(opt, VL_OPT_TYPE_LONG) || v->assign_ != VL_ASSIGN_INLINE) { /* GCOVR_EXCL_BR_LINE: style false short-circuit */
      return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name,
                           "long inline form is disabled");
    }
    return vl_parse_assign_value_(v, opt, eq + 1, flag_index);
  }

  opt = vl_option_find_(v, name);
  if (!opt) {
    return vl_error_add_(v, VL_ERROR_UNKNOWN_OPTION, flag_index, name,
                         "unknown option");
  }
  if (opt->value == VL_OPTION_VALUE_COMMAND) {
    return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name,
                         "command is given after --");
  }
  /* GCOVR_EXCL_BR_START — style false short-circuit */
  if (vl_parse_has_style_(opt, VL_OPT_TYPE_LONG) &&
      opt->value == VL_OPTION_VALUE_BOOL) {
    /* Bare boolean flag: presence means true, in any assign mode. */
    return vl_parse_bool_value_(v, opt, true, flag_index);
  }
  /* GCOVR_EXCL_BR_STOP */
  if (!vl_parse_has_style_(opt, VL_OPT_TYPE_LONG) || v->assign_ != VL_ASSIGN_SEPARATE) { /* GCOVR_EXCL_BR_LINE: style false short-circuit */
    return vl_error_add_(v, VL_ERROR_DISABLED_FORM, flag_index, opt->name,
                         "long separate form is disabled");
  }
  if (!vl_parse_consumable_next_value_(argc, argv, *index, opt)) {
    return vl_error_add_(v, VL_ERROR_MISSING_VALUE, flag_index, opt->name,
                         "missing option value");
  }
  ++(*index);
  return vl_parse_assign_value_(v, opt, argv[*index], flag_index);
}
