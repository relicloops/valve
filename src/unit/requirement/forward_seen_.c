#include "../../private.h"

#include <string.h>

bool vl_requirement_forward_seen_(const valve_t *v, size_t before,
                                  const vl_option_t *source,
                                  const vl_option_t *target) {
  for (size_t i = 0; i < before; ++i) {
    const valve_requirement_t *previous = &v->requirements_[i];
    if (previous->source == source &&
        strcmp(previous->target->name, target->name) == 0)
      return true;
  }
  return false;
}
