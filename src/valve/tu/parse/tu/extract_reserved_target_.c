#include "../private.h"

/* Returns 0 on success, -1 on form mismatch / unexpected value.
 * On success: *target_out is NULL or points into argv (do not free).
 * *consumed_extra is true when argv[i+1] was consumed as the target. */
int vl_parse_extract_reserved_target_(valve_t *v, vl_reserved_kind_t kind,
                                      vl_parse_reserved_form_t form, int argc,
                                      char **argv, int i,
                                      const char *target_in,
                                      const char **target_out,
                                      bool *consumed_extra) {
  *target_out = nullptr;
  *consumed_extra = false;

  if (kind != VL_RESERVED_HELP) {
    if (target_in != nullptr) {
      (void)vl_error_add_(v, VL_ERROR_UNEXPECTED_ARGUMENT, i, argv[i],
                          "reserved token does not accept a value");
      return -1;
    }
    return 0;
  }

  if (form == VL_PARSE_RESERVED_EQ) {
    if (v->assign_ != VL_ASSIGN_INLINE) {
      (void)vl_error_add_(v, VL_ERROR_DISABLED_FORM, i, argv[i],
                          "help target requires separate form");
      return -1;
    }
    if (target_in[0] == '\0') {
      (void)vl_error_add_(v, VL_ERROR_MISSING_VALUE, i, argv[i],
                          "missing help target after '='");
      return -1;
    }
    *target_out = target_in;
    return 0;
  }

  if (form == VL_PARSE_RESERVED_JOINED) {
    *target_out = target_in;
    return 0;
  }

  /* GCOVR_EXCL_BR_START — null/empty next short-circuit */
  if (v->assign_ == VL_ASSIGN_SEPARATE && i + 1 < argc &&
      argv[i + 1] && argv[i + 1][0] != '\0' &&
      !vl_parse_looks_like_option_(argv[i + 1])) {
    *target_out = argv[i + 1];
    *consumed_extra = true;
  }
  /* GCOVR_EXCL_BR_STOP */
  return 0;
}
