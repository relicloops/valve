#include "../../valve_private.h"

bool options_conflicts_valid_(const vl_option_t *const *globals,
                              size_t global_count,
                              const vl_option_t *const *parents,
                              size_t parent_count,
                              const vl_option_t *const *options, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    const vl_option_t *option = options[i];

    if (option->conflict_count > 0 && !option->conflicts)
      return false;

    const size_t conflict_count = option_conflict_count_(option);
    for (size_t j = 0; j < conflict_count; ++j) {
      const vl_option_t *conflict = option->conflicts[j];

      if (!conflict || conflict == option)
        return false;
      if (!option_visible_(conflict, globals, global_count, parents,
                           parent_count, options, count))
        return false;

      for (size_t k = 0; k < j; ++k) {
        if (option->conflicts[k] == conflict)
          return false;
      }
    }
  }

  return true;
}
