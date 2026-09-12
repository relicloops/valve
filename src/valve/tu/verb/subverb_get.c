#include "../../private.h"

const char *vl_subverb_get(const valve_t *v) {
  if (!v || !v->active_subverb_)
    return nullptr;

  return v->active_subverb_->name;
}
