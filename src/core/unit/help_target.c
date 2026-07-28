#include "../valve_private.h"

const char *vl_help_target(const valve_t *v) {
  if (!v)
    return NULL;
  return v->help_target_;
}
