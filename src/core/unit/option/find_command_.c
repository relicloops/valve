#include "../../valve_private.h"

const vl_option_t *vl_option_find_command_(const valve_t *v) {
  if (!v) /* GCOVR_EXCL_BR_LINE: parser always passes its live handle */
    return nullptr; /* GCOVR_EXCL_LINE */

  if (v->active_subverb_) {
    for (size_t i = 0; i < v->active_subverb_->option_count; ++i) {
      if (v->active_subverb_->options[i].value == VL_OPTION_VALUE_COMMAND)
        return &v->active_subverb_->options[i];
    }
  }

  if (v->active_verb_) {
    for (size_t i = 0; i < v->active_verb_->option_count; ++i) {
      if (v->active_verb_->options[i].value == VL_OPTION_VALUE_COMMAND)
        return &v->active_verb_->options[i];
    }
  }

  for (size_t i = 0; i < v->option_count_; ++i) {
    if (v->options_[i].value == VL_OPTION_VALUE_COMMAND)
      return &v->options_[i];
  }

  return nullptr;
}
