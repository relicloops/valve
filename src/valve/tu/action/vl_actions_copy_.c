#include "../../private.h"

#include <stdlib.h>

int vl_actions_copy_(valve_t *v, const vl_executable_t *settings) {
  size_t count = action_count_(settings->actions, settings->action_count);

  v->actions_ = nullptr;
  v->action_run_ = nullptr;
  v->action_count_ = 0;

  if (count == 0)
    return 0;

  /* options_copy_ wants a pointer table; gather the embedded options. */
  const vl_option_t **table = calloc(count, sizeof *table);
  if (!table)
    return -1;

  void (**run)(const valve_t *) = calloc(count, sizeof *run);
  if (!run) {
    free(table);
    return -1;
  }

  for (size_t i = 0; i < count; ++i) {
    table[i] = &settings->actions[i]->option;
    run[i] = settings->actions[i]->run;
  }

  int rc = options_copy_(table, count, &v->actions_, &v->action_count_);
  free(table);
  if (rc != 0) {
    free(run);
    return -1;
  }

  v->action_run_ = run;
  return 0;
}
