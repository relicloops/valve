#include "../valve_private.h"

#include <stdlib.h>

static void *target_ptr_(const vl_option_t *opt) {
  if (!opt || !opt->data || opt->target == VL_TARGET_NONE) /* GCOVR_EXCL_BR_LINE: null opt / NONE short-circuit */
    return NULL;
  return (char *)opt->data + opt->offset;
}

static void clear_option_(const vl_option_t *opt) {
  void *target = target_ptr_(opt);

  if (!target)
    return;

  switch (opt->target) { /* GCOVR_EXCL_BR_LINE: NONE unreachable — target_ptr_ returns NULL first */
  case VL_TARGET_NONE: /* GCOVR_EXCL_BR_LINE: target_ptr_ returns NULL for NONE first */
    return; /* GCOVR_EXCL_LINE */
  case VL_TARGET_STRING:
    free(*(char **)target);
    *(char **)target = NULL;
    return;
  case VL_TARGET_INT:
    *(int *)target = 0;
    return;
  case VL_TARGET_INT64:
    *(int64_t *)target = 0;
    return;
  case VL_TARGET_DOUBLE:
    *(double *)target = 0.0;
    return;
  case VL_TARGET_BOOL:
    *(bool *)target = false;
    return;
  case VL_TARGET_COMMAND:
    *(vl_command_t *)target = (vl_command_t){0};
    return;
  case VL_TARGET_VALUE:
  case VL_TARGET_TOGGLE:
    vl_value_clear((vl_value_t *)target);
    return;
  }
}

static void clear_options_(const vl_option_t *const *options,
                           size_t option_count) {
  size_t n = option_count_(options, option_count);

  for (size_t i = 0; i < n; ++i)
    clear_option_(options[i]);
}

static void clear_verbs_(const vl_verb_t *const *verbs, size_t verb_count) {
  size_t n = verb_table_count_(verbs, verb_count);

  for (size_t i = 0; i < n; ++i) {
    const vl_verb_t *verb = verbs[i];
    if (!verb)
      continue;
    clear_options_(verb->options, verb->option_count);
    clear_verbs_(verb->verbs, verb->verb_count);
  }
}

void vl_targets_clear(const vl_executable_t *settings) {
  if (!settings)
    return;

  clear_options_(settings->options, settings->option_count);
  clear_verbs_(settings->verbs, settings->verb_count);
}
