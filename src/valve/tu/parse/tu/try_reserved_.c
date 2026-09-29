#include "../private.h"

/* Fire argv[i] as a reserved token when it is one. `check_user_option` lets
 * a schema that overrides a reserved name (VL_BEHAVIOR_ALLOW_OVERRIDE_RESERVED)
 * keep its own option; it is false only where argv[i] is a bare token that
 * cannot name a user option. Returns VL_PARSE_CONTINUE when argv[i] is not
 * reserved, 0 when it fired, -1 when the target was invalid. */
int vl_parse_try_reserved_(valve_t *v, int argc, char **argv, int i, bool check_user_option) {

  vl_parse_reserved_form_t form = VL_PARSE_RESERVED_BARE;
  const char *target_in = nullptr;

  vl_reserved_kind_t k = vl_parse_reserved_token_(argv[i], &form, &target_in);
  if (k == VL_RESERVED_NONE) {
    return VL_PARSE_CONTINUE;
  }
  if (check_user_option && vl_parse_user_has_option_for_token_(v, argv[i])) {
    return VL_PARSE_CONTINUE;
  }

  const char *target = nullptr;
  bool consumed = false;
  if (vl_parse_extract_reserved_target_(v, k, form, argc, argv, i, target_in, &target, &consumed) != 0) {
    return -1;
  }

  return vl_parse_dispatch_reserved_target_(v, k, target, i);
}
