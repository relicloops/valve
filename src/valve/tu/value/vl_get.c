#include "../../private.h"

#include <string.h>

const vl_value_t *vl_get(const valve_t *v, const char *key) {
  if (!v || !key)
    return nullptr;

  for (size_t i = 0; i < v->result_count_; ++i) {
    if (strcmp(v->results_[i].key, key) == 0) {
      return &v->results_[i].value;
    }
  }

  return nullptr;
}