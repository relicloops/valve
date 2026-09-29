#include "../private.h"

#include <stdlib.h>
#include <string.h>

int vl_parse_dispatch_reserved_target_(valve_t *v, vl_reserved_kind_t kind, const char *target, int argv_index) {

  if (kind == VL_RESERVED_HELP && target && target[0] != '\0') { /* GCOVR_EXCL_BR_LINE: empty-target short-circuit */
    vl_help_internal_t res = {0};
    if (!vl_help_resolve_internal_(v, target, &res)) {
      char msg[192];

      (void)vl_error_add_(v,
                          VL_ERROR_INVALID_VALUE,
                          argv_index,
                          target,
                          vl_parse_unknown_help_message_(v, target, msg, sizeof msg));

      return -1;
    }

    free(v->help_target_);
    v->help_target_ = strdup(target);
  } else {
    free(v->help_target_);
    v->help_target_ = nullptr;
  }

  vl_parse_dispatch_reserved_(v, kind);

  return 0;
}
