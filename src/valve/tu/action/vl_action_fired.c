#include "../../private.h"

const char *vl_action_fired(const valve_t *v) {
  if (!v || !v->action_fired_)
    return nullptr;
  return v->action_fired_->name;
}
