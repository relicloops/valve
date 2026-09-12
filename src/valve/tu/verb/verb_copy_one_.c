#include "../../private.h"

#include <stdlib.h>
#include <string.h>

static int verb_copy_one_(const vl_verb_t *src, valve_verb_t *dst) {
  size_t local_count = option_count_(src->options, src->option_count);
  size_t sub_count = verb_table_count_(src->verbs, src->verb_count);

  dst->name = strdup(src->name);
  if (!dst->name)
    return -1;

  if (src->description) {
    dst->description = strdup(src->description);
    if (!dst->description)
      return -1;
  }
  if (src->usage) {
    dst->usage = strdup(src->usage);
    if (!dst->usage)
      return -1;
  }

  if (options_copy_(src->options, local_count, &dst->options,
                    &dst->option_count) != 0)
    return -1;

  if (sub_count > 0 &&
      verbs_copy_(src->verbs, sub_count, &dst->verbs, &dst->verb_count) != 0)
    return -1;

  return 0;
}

int verbs_copy_(const vl_verb_t *const *src, size_t count, valve_verb_t **dst,
                size_t *dst_count) {
  valve_verb_t *verbs = NULL;

  *dst = NULL;
  *dst_count = 0;

  if (count == 0)
    return 0;

  verbs = calloc(count, sizeof(valve_verb_t));
  if (!verbs)
    return -1;

  for (size_t i = 0; i < count; ++i) {
    if (verb_copy_one_(src[i], &verbs[i]) != 0) {
      /* calloc'd array: clearing untouched tail entries is safe. */
      verb_array_clear_(verbs, count);
      free(verbs);
      return -1;
    }
  }

  *dst = verbs;
  *dst_count = count;
  return 0;
}
