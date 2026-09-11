#include "../private.h"

void vl_errors_foreach(const valve_t *v, vl_error_fn fn, void *userdata) {
  if (!v || !fn)
    return;

  for (size_t i = 0; i < v->error_count_; ++i)
    fn(&v->errors_[i], userdata);
}
