#include "../../private.h"

size_t action_count_(const vl_executable_action_t *const *actions,
                     size_t count) {
  size_t n = 0;

  if (!actions)
    return 0;

  if (count > 0)
    return count;

  while (actions[n])
    ++n;

  return n;
}
