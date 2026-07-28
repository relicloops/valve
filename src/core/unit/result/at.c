#include "../../valve_private.h"

const vl_result_t *vl_result_at(const valve_t *v, const size_t index) {

  if (!v || index >= v->result_count_) {
    return NULL;
  }

  return &v->results_[index];
}