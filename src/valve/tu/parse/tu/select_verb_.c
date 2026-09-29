#include "../private.h"

/* Resolve argv[1] as a verb (and argv[2] as a sub-verb when the verb owns a
 * table), setting `*start` to the first option token. A reserved token in
 * either slot fires instead. Returns VL_PARSE_CONTINUE to go on parsing
 * options, 0 when a reserved token fired, -1 on error. */
int vl_parse_select_verb_(valve_t *v, int argc, char **argv, int *start) {
  const char *command_name = argc > 1 ? argv[1] : nullptr; /* GCOVR_EXCL_BR_LINE: argc>1 always here */

  /* GCOVR_EXCL_BR_START — null argv[1] / empty-command short-circuit */
  if (!command_name || command_name[0] == '\0' ||
      vl_parse_looks_like_option_(command_name)) {
    if (command_name) { /* GCOVR_EXCL_BR_LINE: null command_name */
      int rc = vl_parse_try_reserved_(v, argc, argv, 1, true);
      if (rc != VL_PARSE_CONTINUE)
        return rc;
    }
    (void)vl_error_add_(v, VL_ERROR_MISSING_COMMAND, 1, nullptr,
                        "missing verb");
    return -1;
  }
  /* GCOVR_EXCL_BR_STOP */

  v->active_verb_ = vl_verb_find_(v, command_name);
  if (!v->active_verb_) {
    /* A bare token cannot name a user option, so no override check. */
    int rc = vl_parse_try_reserved_(v, argc, argv, 1, false);
    if (rc != VL_PARSE_CONTINUE)
      return rc;
    (void)vl_error_add_(v, VL_ERROR_UNKNOWN_COMMAND, 1, command_name,
                        "unknown verb");
    return -1;
  }

  *start = 2;

  if (v->active_verb_->verb_count > 0)
    return vl_parse_select_subverb_(v, argc, argv, start);

  return VL_PARSE_CONTINUE;
}
