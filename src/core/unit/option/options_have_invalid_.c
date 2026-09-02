#include "../../valve_private.h"

bool options_have_invalid_(const vl_option_t *const *options, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (!options[i])
      return true;
    if (!option_name_valid_(options[i]->name) ||
        !short_name_valid_(options[i]->short_name) ||
        !option_type_valid_(options[i]->type) ||
        !option_value_valid_(options[i]->value) ||
        !option_repeat_valid_(options[i]->repeat) ||
        !target_valid_(options[i]->target) ||
        !option_command_valid_(options[i]))
      return true;
    if ((options[i]->type & VL_OPT_TYPE_SHORT) && options[i]->short_name == '\0')
      return true;
    if ((options[i]->type & VL_OPT_TYPE_TOGGLE) &&
        (!options[i]->toggle_ref || options[i]->toggle_ref[0] == '\0'))
      return true;
  }

  return false;
}
