#include "../../private.h"

const vl_option_t *option_mapped_(
    const vl_option_t *source, const vl_option_t *const *source_globals,
    const vl_option_t *copied_globals, size_t global_count,
    const vl_option_t *const *source_parents,
    const vl_option_t *copied_parents, size_t parent_count,
    const vl_option_t *const *source_options,
    const vl_option_t *copied_options, size_t option_count) {
  for (size_t i = 0; i < option_count; ++i) {
    if (source_options[i] == source)
      return &copied_options[i];
  }
  for (size_t i = 0; i < parent_count; ++i) {
    if (source_parents[i] == source)
      return &copied_parents[i];
  }
  for (size_t i = 0; i < global_count; ++i) {
    if (source_globals[i] == source)
      return &copied_globals[i];
  }
  return nullptr;
}
