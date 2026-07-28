#include "../valve_private.h"

static int check_options_(valve_t *v, const vl_option_t *options,
                          size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (!options[i].required || vl_has(v, options[i].name))
      continue;

    if (vl_error_add_(v, VL_ERROR_MISSING_REQUIRED, 0, options[i].name,
                      "missing required option") != 0)
      return -1;
  }

  return 0;
}

/* Post-parse pass: every in-scope option (globals + active verb chain)
 * marked required must appear in the results. Skipped by the caller when a
 * reserved token fired. */
int vl_required_check_(valve_t *v) {
  if (check_options_(v, v->options_, v->option_count_) != 0)
    return -1;

  if (v->active_verb_ &&
      check_options_(v, v->active_verb_->options,
                     v->active_verb_->option_count) != 0)
    return -1;

  if (v->active_subverb_ &&
      check_options_(v, v->active_subverb_->options,
                     v->active_subverb_->option_count) != 0)
    return -1;

  return 0;
}
