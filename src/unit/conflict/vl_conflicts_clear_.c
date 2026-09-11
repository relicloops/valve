#include "../../private.h"

#include <stdlib.h>

void vl_conflicts_clear_(valve_t *v) {
  if (!v)
    return;

  free(v->conflicts_);
  v->conflicts_ = nullptr;
  v->conflict_count_ = 0;
  v->conflict_cap_ = 0;
}
