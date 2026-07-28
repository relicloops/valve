#include "../../valve_private.h"

const char *vl_verb_get(const valve_t *v) {
  if (!v || !v->active_verb_)
    return NULL;

  return v->active_verb_->name;
}
