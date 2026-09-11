#include "../../private.h"

#include <string.h>

const vl_result_t *vl_result_find_(const valve_t *v, const char *name) {
  for (size_t i = 0; i < v->result_count_; ++i) {
    if (strcmp(v->results_[i].key, name) == 0)
      return &v->results_[i];
  }
  return nullptr;
}
