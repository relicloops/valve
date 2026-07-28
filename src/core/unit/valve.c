#include "../valve_private.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool target_valid_(vl_target_t target) {
  switch (target) {
  case VL_TARGET_NONE:
  case VL_TARGET_STRING:
  case VL_TARGET_INT:
  case VL_TARGET_INT64:
  case VL_TARGET_DOUBLE:
  case VL_TARGET_BOOL:
  case VL_TARGET_VALUE:
  case VL_TARGET_TOGGLE:
    return true;
  }

  return false;
}

bool short_name_valid_(char short_name) {
  return short_name == '\0' || (short_name != '-' && short_name != '=');
}

static int settings_meta_copy_(valve_t *v, const vl_executable_t *settings) {
  if (settings->program_name) {
    v->program_name_ = strdup(settings->program_name);
    if (!v->program_name_)
      return -1;
  }
  if (settings->program_version) {
    v->program_version_ = strdup(settings->program_version);
    if (!v->program_version_)
      return -1;
  }
  if (settings->description) {
    v->description_ = strdup(settings->description);
    if (!v->description_)
      return -1;
  }
  if (settings->usage) {
    v->usage_ = strdup(settings->usage);
    if (!v->usage_)
      return -1;
  }
  if (settings->logo) {
    v->logo_ = strdup(settings->logo);
    if (!v->logo_)
      return -1;
  }
  v->behavior_ = settings->behavior;
  v->on_help_ = settings->on_help;
  v->on_version_ = settings->on_version;
  v->on_valve_ = settings->on_valve;
  return 0;
}

bool assign_valid_(vl_assign_t assign) {
  return assign == VL_ASSIGN_INLINE || assign == VL_ASSIGN_SEPARATE;
}

valve_t *vl_create(const vl_executable_t *settings) {
  if (!vl_schema_validate_(settings))
    return NULL;

  valve_t *v = calloc(1, sizeof(valve_t));

  size_t option_count =
      settings ? option_count_(settings->options, settings->option_count) : 0;
  size_t verb_count = verb_count_(settings);

  if (!v)
    return NULL;

  v->assign_ = settings ? settings->assign : VL_ASSIGN_INLINE;

  if (settings && settings_meta_copy_(v, settings) != 0) {
    vl_settings_meta_clear_(v);
    free(v);
    return NULL;
  }

  if (options_copy_(settings ? settings->options : NULL, option_count,
                    &v->options_, &v->option_count_) != 0) {
    vl_settings_meta_clear_(v);
    free(v);
    return NULL;
  }

  if (verb_count > 0 &&
      verbs_copy_(settings->verbs, verb_count, &v->verbs_,
                  &v->verb_count_) != 0) {
    vl_options_clear_(v);
    vl_settings_meta_clear_(v);
    free(v);
    return NULL;
  }

  if (settings && vl_conflicts_copy_(v, settings) != 0) {
    vl_conflicts_clear_(v);
    vl_verbs_clear_(v);
    vl_options_clear_(v);
    vl_settings_meta_clear_(v);
    free(v);
    return NULL;
  }

  if (settings && vl_requirements_copy_(v, settings) != 0) {
    vl_requirements_clear_(v);
    vl_conflicts_clear_(v);
    vl_verbs_clear_(v);
    vl_options_clear_(v);
    vl_settings_meta_clear_(v);
    free(v);
    return NULL;
  }

  vl_color_init(settings ? settings->color : VAL_COLOR_AUTO);
  return v;
}

void vl_destroy(valve_t *v) {
  if (!v)
    return;
  vl_requirements_clear_(v);
  vl_conflicts_clear_(v);
  vl_options_clear_(v);
  vl_verbs_clear_(v);
  vl_results_clear_(v);
  vl_errors_clear_(v);
  vl_settings_meta_clear_(v);
  free(v);
}
