#include "../../private.h"

bool option_value_valid_(vl_option_value_t value) {

  switch (value) {

  case VL_OPTION_VALUE_AUTO:
  case VL_OPTION_VALUE_STRING:
  case VL_OPTION_VALUE_INT:
  case VL_OPTION_VALUE_DOUBLE:
  case VL_OPTION_VALUE_NUMBER:
  case VL_OPTION_VALUE_KV:
  case VL_OPTION_VALUE_TOGGLE:
  case VL_OPTION_VALUE_ARRAY:
  case VL_OPTION_VALUE_BOOL:
  case VL_OPTION_VALUE_DOT_NOTATION:
  case VL_OPTION_VALUE_TIME:
  case VL_OPTION_VALUE_COMMAND:

    return true;
  }

  return false;
}
