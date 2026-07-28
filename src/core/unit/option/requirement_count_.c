#include "../../valve_private.h"

size_t option_requirement_count_(const vl_option_t *option) {
  if (!option || !option->requires)
    return 0;

  if (option->require_count > 0)
    return option->require_count;

  size_t count = 0;
  while (option->requires[count])
    ++count;
  return count;
}
