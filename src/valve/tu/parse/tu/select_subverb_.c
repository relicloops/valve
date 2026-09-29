#include "../private.h"

int vl_parse_select_subverb_(valve_t *v, int argc, char **argv, int *start) {

  const char *subverb_name = argc > 2 ? argv[2] : nullptr;
  if (!subverb_name || subverb_name[0] == '\0' || vl_parse_looks_like_option_(subverb_name)) {
    if (subverb_name) {
      int rc = vl_parse_try_reserved_(v, argc, argv, 2, true);
      if (rc != VL_PARSE_CONTINUE) {
        return rc;
      }
    }

    (void)vl_error_add_(v, VL_ERROR_MISSING_COMMAND, 2, v->active_verb_->name, "missing sub-verb");

    return -1;
  }

  v->active_subverb_ = verb_find_in_(v->active_verb_->verbs, v->active_verb_->verb_count, subverb_name);
  if (!v->active_subverb_) {
    (void)vl_error_add_(v, VL_ERROR_UNKNOWN_COMMAND, 2, subverb_name, "unknown sub-verb");

    return -1;
  }

  *start = 3;

  return VL_PARSE_CONTINUE;
}
