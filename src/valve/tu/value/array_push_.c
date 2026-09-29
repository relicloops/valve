#include "../../private.h"

#include <stdlib.h>

int vl_value_array_push_(vl_array_t *list, vl_value_t *value) {
  size_t next_count, bytes;
  /* GCOVR_EXCL_BR_START — size_t wrap on pathological count */
  if (__builtin_add_overflow(list->count, (size_t)1, &next_count) ||
      __builtin_mul_overflow(next_count, sizeof(vl_value_t), &bytes))
    return -1; /* GCOVR_EXCL_LINE */
  /* GCOVR_EXCL_BR_STOP */

  vl_value_t *next = realloc((vl_value_t *)list->items, bytes);

  if (!next)
    return -1;

  next[list->count] = *value;
  list->items = next;
  ++list->count;
  *value = (vl_value_t){0};
  return 0;
}
