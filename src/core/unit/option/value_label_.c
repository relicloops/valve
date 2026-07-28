#include "../../valve_private.h"

const char *option_value_label_(const vl_option_t *option) {
  switch (option->value) {
  case VL_OPTION_VALUE_STRING:
    return "string";
  case VL_OPTION_VALUE_INT:
    return "int";
  case VL_OPTION_VALUE_DOUBLE:
    return "double";
  case VL_OPTION_VALUE_NUMBER:
    return "number";
  case VL_OPTION_VALUE_KV:
    return "kv";
  case VL_OPTION_VALUE_ARRAY:
    return "array";
  case VL_OPTION_VALUE_BOOL:
    return "bool";
  case VL_OPTION_VALUE_TIME:
    return "duration";
  case VL_OPTION_VALUE_DOT_NOTATION:
    switch (option->target) { /* GCOVR_EXCL_BR_LINE: TOGGLE/VALUE/NONE share default */
    case VL_TARGET_STRING:
      return "string";
    case VL_TARGET_INT:
    case VL_TARGET_INT64:
      return "int";
    case VL_TARGET_DOUBLE:
      return "double";
    case VL_TARGET_BOOL:
      return "bool";
    case VL_TARGET_NONE:
    case VL_TARGET_VALUE:
    case VL_TARGET_TOGGLE:
    default:
      return "value";
    }
  case VL_OPTION_VALUE_AUTO:
  case VL_OPTION_VALUE_TOGGLE:
  default:
    return "value";
  }
}
