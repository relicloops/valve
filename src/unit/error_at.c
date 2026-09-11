#include "../private.h"

const vl_error_t *vl_error_at(const valve_t *v, size_t index) {
  if (!v || index >= v->error_count_)
    return NULL;
  return &v->errors_[index];
}
