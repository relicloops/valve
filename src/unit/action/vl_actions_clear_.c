#include "../../private.h"

#include <stdlib.h>

void vl_actions_clear_(valve_t *v) {
  if (!v)
    return;

  option_array_free_(v->actions_, v->action_count_);
  free(v->action_run_);
  v->actions_ = nullptr;
  v->action_run_ = nullptr;
  v->action_count_ = 0;
  v->active_action_ = nullptr;
  v->action_fired_ = nullptr;
}
