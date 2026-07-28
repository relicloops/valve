#include "../valve_private.h"

bool vl_reserved_fired(const valve_t *v) {
  if (!v)
    return false;
  return v->reserved_fired_;
}
