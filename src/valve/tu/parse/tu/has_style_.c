#include "../private.h"

bool vl_parse_has_style_(const vl_option_t *opt, vl_opt_type_t style) {

  /* GCOVR_EXCL_BR_START — opt is non-null at every call site */
  return opt && (opt->type & style);
  /* GCOVR_EXCL_BR_STOP */
}
