#include "../../valve_private.h"

bool option_repeat_valid_(vl_option_repeat_t repeat) {

  switch (repeat) {
  case VL_OPTION_REPEAT_ERROR:
  case VL_OPTION_REPEAT_ARRAY:
    return true;
  }

  return false;
}