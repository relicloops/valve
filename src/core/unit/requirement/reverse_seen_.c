#include "../../valve_private.h"

#include <string.h>

bool vl_requirement_reverse_seen_(const valve_t *v, size_t before,
                                  const vl_option_t *target,
                                  const vl_option_t *source) {
  for (size_t i = 0; i < before; ++i) {
    const valve_requirement_t *previous = &v->requirements_[i];
    if (previous->target == target &&
        strcmp(previous->source->name, source->name) == 0)
      return true;
  }
  return false;
}
