#include "../../valve_private.h"

size_t options_command_count_(const vl_option_t *const *options,
                              size_t count) {
  size_t commands = 0;

  if (!options)
    return 0;

  for (size_t i = 0; i < count; ++i) {
    if (options[i] && options[i]->value == VL_OPTION_VALUE_COMMAND)
      ++commands;
  }

  return commands;
}
