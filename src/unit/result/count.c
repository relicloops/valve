#include "../../private.h"

size_t vl_result_count(const valve_t *v) {
  if (!v)
    return 0;
  return v->result_count_;
}