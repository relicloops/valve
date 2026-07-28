#include "../valve_private.h"

#include <stdlib.h>

void vl_options_clear_(valve_t *v) {
  if (!v)
    return;

  for (size_t i = 0; i < v->option_count_; ++i) {
    free((char *)v->options_[i].name);
    free((char *)v->options_[i].description);
    free((char *)v->options_[i].usage);
  }
  free(v->options_);
  v->options_ = NULL;
  v->option_count_ = 0;
}



void vl_settings_meta_clear_(valve_t *v) {
  if (!v)
    return;

  free(v->program_name_);
  free(v->program_version_);
  free(v->description_);
  free(v->usage_);
  free(v->logo_);
  free(v->help_target_);
  v->program_name_ = NULL;
  v->program_version_ = NULL;
  v->description_ = NULL;
  v->usage_ = NULL;
  v->logo_ = NULL;
  v->help_target_ = NULL;
  v->behavior_ = 0;
  v->on_help_ = NULL;
  v->on_version_ = NULL;
  v->on_valve_ = NULL;
}

void vl_errors_clear_(valve_t *v) {
  if (!v)
    return;

  for (size_t i = 0; i < v->error_count_; ++i) {
    free((char *)v->errors_[i].key);
    free((char *)v->errors_[i].message);
  }
  free(v->errors_);
  v->errors_ = NULL;
  v->error_count_ = 0;
  v->error_cap_ = 0;
}
