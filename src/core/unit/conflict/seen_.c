#include "../../valve_private.h"

#include <string.h>

bool vl_conflict_seen_(const valve_t *v, size_t before,
                       const vl_option_t *option,
                       const vl_option_t *other) {
  for (size_t i = 0; i < before; ++i) {
    const vl_option_t *previous =
        vl_conflict_other_(&v->conflicts_[i], option);
    if (previous && strcmp(previous->name, other->name) == 0)
      return true;
  }
  return false;
}
