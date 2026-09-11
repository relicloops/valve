#include "../../private.h"

int vl_conflicts_copy_table_(
    valve_t *v, const vl_option_t *const *source_globals,
    const vl_option_t *copied_globals, size_t global_count,
    const vl_option_t *const *source_parents,
    const vl_option_t *copied_parents, size_t parent_count,
    const vl_option_t *const *source_options,
    const vl_option_t *copied_options, size_t option_count,
    const valve_verb_t *verb, const valve_verb_t *subverb) {
  for (size_t i = 0; i < option_count; ++i) {
    const vl_option_t *source = source_options[i];
    const vl_option_t *copied = &copied_options[i];
    const size_t conflict_count = option_conflict_count_(source);

    for (size_t j = 0; j < conflict_count; ++j) {
      const vl_option_t *target = option_mapped_(
          source->conflicts[j], source_globals, copied_globals, global_count,
          source_parents, copied_parents, parent_count, source_options,
          copied_options, option_count);
      /* GCOVR_EXCL_BR_START — mapped target always present after schema validate */
      if (!target ||
          vl_conflict_add_(v, copied, target, verb, subverb) != 0)
        return -1;
      /* GCOVR_EXCL_BR_STOP */
    }
  }
  return 0;
}
