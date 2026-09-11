#include "../../private.h"

#include <stdlib.h>

void vl_requirements_clear_(valve_t *v) {
  if (!v)
    return;

  free(v->requirements_);
  v->requirements_ = nullptr;
  v->requirement_count_ = 0;
  v->requirement_cap_ = 0;
}
