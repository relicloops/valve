#include "../private.h"

#include <stdlib.h>

void vl_options_clear_(valve_t *v) {

  if (!v) {
    return;
  }

  for (size_t i = 0; i < v->option_count_; ++i) {
    free((char *)v->options_[i].name);
    free((char *)v->options_[i].description);
    free((char *)v->options_[i].usage);
  }

  free(v->options_);
  v->options_ = nullptr;
  v->option_count_ = 0;
}

void vl_settings_meta_clear_(valve_t *v) {

  if (!v) {
    return;
  }

  free(v->program_name_);
  free(v->program_version_);
  free(v->description_);
  free(v->usage_);
  free(v->logo_);
  free(v->help_target_);
  v->program_name_ = nullptr;
  v->program_version_ = nullptr;
  v->description_ = nullptr;
  v->usage_ = nullptr;
  v->logo_ = nullptr;
  v->help_target_ = nullptr;
  v->behavior_ = 0;
  v->on_help_ = nullptr;
  v->on_version_ = nullptr;
  v->on_valve_ = nullptr;
}

void vl_errors_clear_(valve_t *v) {

  if (!v) {
    return;
  }

  for (size_t i = 0; i < v->error_count_; ++i) {
    free((char *)v->errors_[i].key);
    free((char *)v->errors_[i].message);
  }

  free(v->errors_);
  v->errors_ = nullptr;
  v->error_count_ = 0;
  v->error_cap_ = 0;
}
