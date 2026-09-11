#include "../../private.h"

bool option_type_valid_(vl_opt_type_t style) {

  if (style & ~(VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT | VL_OPT_TYPE_TOGGLE))
    return false;
  if ((style & VL_OPT_TYPE_TOGGLE) && (style & (VL_OPT_TYPE_LONG | VL_OPT_TYPE_SHORT)))
    return false;

  return true;
  
}