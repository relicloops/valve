#include "../../valve_private.h"

#include <stdlib.h>

void vl_results_clear_(valve_t *v) {
  if (!v)
    return;

  for (size_t i = 0; i < v->result_count_; ++i) {
    free((char *)v->results_[i].key);
    vl_value_clear(&v->results_[i].value);
  }
  free(v->results_);
  v->results_ = NULL;
  v->result_count_ = 0;
  v->result_cap_ = 0;
}