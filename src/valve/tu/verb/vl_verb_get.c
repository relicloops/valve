#include "../../private.h"

const char *vl_verb_get(const valve_t *v) {
  if (!v || !v->active_verb_)
    return nullptr;

  return v->active_verb_->name;
}
