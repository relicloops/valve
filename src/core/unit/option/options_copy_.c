#include "../../valve_private.h"

#include <stdlib.h>
#include <string.h>

int options_copy_(const vl_option_t *const *src, size_t count,
                         vl_option_t **dst, size_t *dst_count) {
  vl_option_t *options = NULL;
  size_t copied = 0;

  *dst = NULL;
  *dst_count = 0;

  if (count == 0)
    return 0;

  options = calloc(count, sizeof(vl_option_t));
  if (!options)
    return -1;

  for (size_t i = 0; i < count; ++i) {
    options[i] = *src[i];
    options[i].name = strdup(src[i]->name);
    options[i].description = NULL;
    options[i].usage = NULL;
    options[i].conflicts = nullptr;
    options[i].conflict_count = 0;
    options[i].requires = nullptr;
    options[i].require_count = 0;
    if (!options[i].name) {
      option_array_free_(options, copied);
      return -1;
    }
    if (src[i]->description) {
      options[i].description = strdup(src[i]->description);
      if (!options[i].description) {
        ++copied;
        option_array_free_(options, copied);
        return -1;
      }
    }
    if (src[i]->usage) {
      options[i].usage = strdup(src[i]->usage);
      if (!options[i].usage) {
        ++copied;
        option_array_free_(options, copied);
        return -1;
      }
    }
    ++copied;
  }

  *dst = options;
  *dst_count = copied;
  return 0;
}
