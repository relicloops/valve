#include "../../private.h"

#include <stdlib.h>

void vl_value_clear(vl_value_t *value) {

  if (!value) return;

  if (value->kind == VL_VALUE_ARRAY) {
    for (size_t i = 0; i < value->as.array.count; ++i) {
      vl_value_clear((vl_value_t *)&value->as.array.items[i]);
    }
    free((vl_value_t *)value->as.array.items);
  } else if (value->kind == VL_VALUE_KV) {
    for (size_t i = 0; i < value->as.kv.count; ++i) {
      free((char *)value->as.kv.pairs[i].key);
      vl_value_clear((vl_value_t *)&value->as.kv.pairs[i].value);
    }
    free((vl_kv_pair_t *)value->as.kv.pairs);
  }

  free((char *)value->raw);
  *value = (vl_value_t){0};
}