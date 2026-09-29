#include "../private.h"

void vl_parse_dispatch_reserved_(valve_t *v, vl_reserved_kind_t kind) {
  v->reserved_fired_ = true;
  switch (kind) { /* GCOVR_EXCL_BR_LINE: NONE/default unreachable */
  case VL_RESERVED_VALVE:
    if (v->on_valve_)
      v->on_valve_(v);
    else
      vl_valve_print_default_(v);
    break;
  case VL_RESERVED_HELP:
    if (v->on_help_)
      v->on_help_(v);
    else
      vl_help_print_default_(v);
    break;
  case VL_RESERVED_VERSION:
    if (v->on_version_)
      v->on_version_(v);
    else
      vl_version_print_default_(v);
    break;
  case VL_RESERVED_NONE: /* GCOVR_EXCL_LINE: dispatch only for real kinds */
  default: /* GCOVR_EXCL_BR_LINE */
    break; /* GCOVR_EXCL_LINE */
  }
}
