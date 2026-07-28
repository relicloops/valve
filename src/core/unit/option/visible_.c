#include "../../valve_private.h"

bool option_visible_(const vl_option_t *option,
                     const vl_option_t *const *globals, size_t global_count,
                     const vl_option_t *const *parents, size_t parent_count,
                     const vl_option_t *const *options, size_t count) {
  return option_in_(option, options, count) ||
         option_in_(option, parents, parent_count) ||
         option_in_(option, globals, global_count);
}
