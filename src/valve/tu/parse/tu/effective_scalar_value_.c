#include "../private.h"

/* Scalar parse kind for an option. For a dot-notation option the name is a
 * grouped path, so the leaf's scalar kind is taken from `.target`; every other
 * option parses according to its own `.value`. */
vl_option_value_t vl_parse_effective_scalar_value_(const vl_option_t *opt) {
  if (!opt) /* GCOVR_EXCL_BR_LINE: null guard */
    return VL_OPTION_VALUE_AUTO; /* GCOVR_EXCL_LINE */
  if (opt->value != VL_OPTION_VALUE_DOT_NOTATION)
    return opt->value;

  switch (opt->target) { /* GCOVR_EXCL_BR_LINE: TOGGLE/VALUE/NONE/default share AUTO */
  case VL_TARGET_INT:
  case VL_TARGET_INT64:
    return VL_OPTION_VALUE_INT;
  case VL_TARGET_DOUBLE:
    return VL_OPTION_VALUE_DOUBLE;
  case VL_TARGET_BOOL:
    return VL_OPTION_VALUE_BOOL;
  case VL_TARGET_STRING:
    return VL_OPTION_VALUE_STRING;
  case VL_TARGET_NONE:
  case VL_TARGET_VALUE:
  case VL_TARGET_TOGGLE:
  default:
    return VL_OPTION_VALUE_AUTO;
  }
}
