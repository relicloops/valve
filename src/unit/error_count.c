#include "../private.h"

size_t vl_error_count(const valve_t *v) {
  if (!v)
    return 0;
  return v->error_count_;
}
