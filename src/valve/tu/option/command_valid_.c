#include "../../private.h"

bool option_command_valid_(const vl_option_t *option) {
  if (!option) /* GCOVR_EXCL_BR_LINE: options_have_invalid_ rejects null first */
    return false; /* GCOVR_EXCL_LINE */

  if (option->value != VL_OPTION_VALUE_COMMAND)
    return true;

  if (option->type != 0 || option->short_name != '\0' || option->toggle_ref)
    return false;
  if (option->repeat != VL_OPTION_REPEAT_ERROR)
    return false;

  return option->target == VL_TARGET_NONE ||
         option->target == VL_TARGET_COMMAND ||
         option->target == VL_TARGET_VALUE;
}
