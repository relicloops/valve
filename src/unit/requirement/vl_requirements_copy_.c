#include "../../private.h"

int vl_requirements_copy_(valve_t *v, const vl_executable_t *settings) {
  const size_t global_count =
      option_count_(settings->options, settings->option_count);

  if (vl_requirements_copy_table_(
          v, nullptr, nullptr, 0, nullptr, nullptr, 0, settings->options,
          v->options_, global_count, nullptr, nullptr) != 0)
    return -1;

  const size_t verb_count = verb_count_(settings);
  for (size_t i = 0; i < verb_count; ++i) {
    const vl_verb_t *source_verb = settings->verbs[i];
    valve_verb_t *copied_verb = &v->verbs_[i];
    const size_t option_count =
        option_count_(source_verb->options, source_verb->option_count);

    if (vl_requirements_copy_table_(
            v, settings->options, v->options_, global_count, nullptr, nullptr,
            0, source_verb->options, copied_verb->options, option_count,
            copied_verb, nullptr) != 0)
      return -1;

    const size_t subverb_count =
        verb_table_count_(source_verb->verbs, source_verb->verb_count);
    for (size_t j = 0; j < subverb_count; ++j) {
      const vl_verb_t *source_subverb = source_verb->verbs[j];
      valve_verb_t *copied_subverb = &copied_verb->verbs[j];
      const size_t suboption_count = option_count_(
          source_subverb->options, source_subverb->option_count);

      if (vl_requirements_copy_table_(
              v, settings->options, v->options_, global_count,
              source_verb->options, copied_verb->options, option_count,
              source_subverb->options, copied_subverb->options,
              suboption_count, copied_verb, copied_subverb) != 0)
        return -1;
    }
  }
  return 0;
}
