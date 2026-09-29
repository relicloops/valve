#include "../private.h"

int vl_parse(valve_t *v, int argc, char **argv) {

  int start = 1;
  int rc;

  if (!v || argc < 0 || !argv) {
    return -1;
  }

  vl_results_clear_(v);
  vl_errors_clear_(v);
  v->active_verb_ = nullptr;
  v->active_subverb_ = nullptr;
  v->reserved_fired_ = false;
  v->active_action_ = nullptr;
  v->action_fired_ = nullptr;
  v->parsed_ = true;
  if (argc <= 1) {
    vl_parse_dispatch_reserved_(v, VL_RESERVED_HELP);

    return 0;
  }

  if (v->action_count_ > 0) {
    rc = vl_parse_action_(v, argc, argv);
    if (rc != VL_PARSE_CONTINUE) {
      return rc;
    }
  }

  if (v->verb_count_ > 0) {
    rc = vl_parse_select_verb_(v, argc, argv, &start);
    if (rc != VL_PARSE_CONTINUE) {
      return rc;
    }
  }

  rc = vl_parse_options_(v, argc, argv, start);
  if (rc != VL_PARSE_CONTINUE) {
    return rc;
  }

  return vl_parse_finish_(v);
}
