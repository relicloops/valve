#include "../private.h"

/* Executable action at argv[1]. The token is parsed with the ordinary option
 * rules while lookup is narrowed to the matched action, then the action is
 * dispatched like a reserved token. Returns VL_PARSE_CONTINUE when argv[1]
 * is not an action, 0 when it fired, -1 on error. */
int vl_parse_action_(valve_t *v, int argc, char **argv) {

  const vl_option_t *action = vl_action_find_(v, argv[1]);
  int i = 1;
  int rc;

  if (!action) {
    return VL_PARSE_CONTINUE;
  }

  v->active_action_ = action;
  if (argv[1][1] == '-') {
    rc = vl_parse_enable_disable_(v, argv[1], i);
    if (rc == VL_PARSE_CONTINUE) {
      rc = vl_parse_long_option_(v, argc, argv, &i);
    }
  } else {
    rc = vl_parse_short_option_(v, argc, argv, &i);
  }

  v->active_action_ = nullptr;
  if (rc < 0) {
    (void)vl_error_add_(v, VL_ERROR_OUT_OF_MEMORY, i, nullptr, "out of memory");

    return -1;
  }

  if (v->error_count_) {
    return -1;
  }

  if (i + 1 < argc) {
    (void)vl_error_add_(v, VL_ERROR_UNEXPECTED_ARGUMENT, i + 1, argv[i + 1], "unexpected argument after action");

    return -1;
  }

  v->reserved_fired_ = true;
  v->action_fired_ = action;
  v->action_run_[action - v->actions_](v);

  return 0;
}
