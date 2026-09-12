#include "../../private.h"

bool options_requirements_valid_(const vl_option_t *const *globals,
                                 size_t global_count,
                                 const vl_option_t *const *parents,
                                 size_t parent_count,
                                 const vl_option_t *const *options,
                                 size_t count) {
  for (size_t i = 0; i < count; ++i) {
    const vl_option_t *option = options[i];

    if (option->require_count > 0 && !option->requires)
      return false;

    const size_t requirement_count = option_requirement_count_(option);
    for (size_t j = 0; j < requirement_count; ++j) {
      const vl_option_t *target = option->requires[j];

      if (!target || target == option)
        return false;
      if (!option_visible_(target, globals, global_count, parents,
                           parent_count, options, count))
        return false;

      for (size_t k = 0; k < j; ++k) {
        if (option->requires[k] == target)
          return false;
      }
    }
  }

  return true;
}
